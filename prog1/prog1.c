#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Step 1: Define what a "Record" (a page in our notebook) looks like.
// Every student will have an ID, a Name, and a GPA.
struct Student {
    int id;
    char name[30];
    float gpa;
};

// These are prototypes. They just tell the computer that these functions exist below.
void storeRecords(const char *filename);
void displayMthRecord(const char *filename, int m);
void deleteRecord(const char *filename, int idToDelete);
void displayAllRecords(const char *filename);


int main() {
    const char *filename = "students.bin"; // Name of our binary file

    printf("--- STEP 1: Creating and storing 3 student records ---\n");
    storeRecords(filename);
    displayAllRecords(filename);

    printf("\n--- STEP 2: Fetching the 2nd record directly (Index 1) ---\n");
    // In coding, we start counting from 0. 
    // Index 0 = 1st record, Index 1 = 2nd record, Index 2 = 3rd record.
    displayMthRecord(filename, 1); 

    printf("\n--- STEP 3: Deleting student with ID 102 ---\n");
    deleteRecord(filename, 102);

    printf("\n--- STEP 4: Checking the file after deletion ---\n");
    displayAllRecords(filename);

    return 0;
}


// FUNCTION 1: Create a file and save 3 students into it
void storeRecords(const char *filename) {
    // "wb" means Write Binary. It creates a new binary file.
    FILE *file = fopen(filename, "wb"); 
    if (file == NULL) {
        printf("Error creating file!\n");
        return;
    }

    // Creating an array of 3 students with data
    struct Student data[3] = {
        {101, "Alice", 3.8},
        {102, "Bob",   3.5},
        {103, "Charlie", 3.9}
    };

    // fwrite writes the entire array into the file in one shot!
    // It says: Take 'data', see how big 1 Student is, write 3 of them, into 'file'.
    fwrite(data, sizeof(struct Student), 3, file);

    fclose(file); // Always close the file when done
    printf("Records saved successfully!\n");
}


// FUNCTION 2: Jump directly to the m-th record using fseek
void displayMthRecord(const char *filename, int m) {
    // "rb" means Read Binary.
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    struct Student record;

    // This is the magic math! 
    // If we want index 'm', we skip 'm' number of students from the start.
    int position = m * sizeof(struct Student);

    // fseek moves the file pointer (our finger) to that exact byte position
    // SEEK_SET means "start counting from the beginning of the file"
    fseek(file, position, SEEK_SET);

    // Now read exactly 1 student record from that new position
    if (fread(&record, sizeof(struct Student), 1, file) == 1) {
        printf("Found! ID: %d, Name: %s, GPA: %.2f\n", record.id, record.name, record.gpa);
    } else {
        printf("Record not found.\n");
    }

    fclose(file);
}


// FUNCTION 3: Delete a record using the Temporary File Trick
void deleteRecord(const char *filename, int idToDelete) {
    FILE *file = fopen(filename, "rb");       // Open original file to read
    FILE *tempFile = fopen("temp.bin", "wb"); // Open temporary file to write

    if (file == NULL || tempFile == NULL) {
        printf("Error opening files!\n");
        return;
    }

    struct Student record;

    // Read the original file, one student at a time, until the end of the file
    while (fread(&record, sizeof(struct Student), 1, file) == 1) {
        // If this student is the one we want to delete, SKIP THEM
        if (record.id == idToDelete) {
            printf("Deleting %s from the records...\n", record.name);
            continue; // Skip the rest of the loop, don't write to tempFile
        }
        
        // Otherwise, copy the student to the temporary file
        fwrite(&record, sizeof(struct Student), 1, tempFile);
    }

    // Must close files before deleting or renaming them!
    fclose(file);
    fclose(tempFile);

    remove(filename);          // Delete the old original file
    rename("temp.bin", filename); // Rename temp.bin to students.bin
}


// HELPER FUNCTION: Just to print whatever is in the file line-by-line
void displayAllRecords(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("File is empty or doesn't exist.\n");
        return;
    }

    struct Student record;
    while (fread(&record, sizeof(struct Student), 1, file) == 1) {
        printf("  [ID: %d | Name: %s | GPA: %.2f]\n", record.id, record.name, record.gpa);
    }
    fclose(file);
}

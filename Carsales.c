#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_CARS 15
#define MAX_NAME_LENGTH 50
#define MAX_FEEDBACK_LENGTH 200
#define FILENAME "sales_data.bin"

// Structure to store car details
typedef struct {
    char model[MAX_NAME_LENGTH];
    double price;
    int year;
    int stock;
} Car;

// Structure to store sale details
typedef struct {
    char customerName[MAX_NAME_LENGTH];
    int customerAge;
    double totalPrice;
    double discount;
    int purchaseCount;
    char purchaseModels[MAX_CARS][MAX_NAME_LENGTH];
    int purchaseQuantities[MAX_CARS];
    double purchasePrices[MAX_CARS];
    char date[MAX_NAME_LENGTH];
    int financeDuration;       // Duration in months
    double monthlyPayment;     // Monthly payment amount
    double totalFinanceAmount; // Total amount payable through finance
} Sale;

// Structure to store customer feedback
typedef struct {
    char model[MAX_NAME_LENGTH];
    int rating;
    char comment[MAX_FEEDBACK_LENGTH];
} Feedback;

// Global variables
Car cars[MAX_CARS];              // Array to store car information
int carCount = 0;                // Number of cars in the inventory

Sale sales[MAX_CARS * 10];       // Array to store sales data
int saleCount = 0;               // Number of sales made

Feedback feedbacks[MAX_CARS * 10]; // Array to store customer feedback
int feedbackCount = 0;           // Number of feedback entries

// Function declarations
void loadSalesData();
void saveSalesData();
void viewCarsStock();
void buyCars();
void viewSalesData();
void viewCustomerFeedback();
void sortCarsByYear();
int findCarIndex(const char *model);
void swapCars(int i, int j);
int getIntInput(const char *prompt);
double getDoubleInput(const char *prompt);
void getStringInput(const char *prompt, char *input, int maxLength);
void displaySalesSummary();
void initializeCars();
double calculateMonthlyPayment(double amount, double annualRate, int months);

int main() {
    printf("Welcome to Ansh Garage\n");  // Welcome message

    // Initialize car data
    initializeCars();

    // Load previous sales data from file
    loadSalesData();

    // Main menu loop
    while (1) {
        printf("\n--- Car Sales Menu ---\n");
        printf("1. View Cars Stock\n");
        printf("2. Buy Cars\n");
        printf("3. View Sales Data\n");
        printf("4. Customer Feedback\n");
        printf("5. Exit\n");
        printf("Choose an option: ");
        
        int choice = getIntInput("");
        switch (choice) {
            case 1:
                viewCarsStock();
                break;
            case 2:
                buyCars();
                break;
            case 3:
                viewSalesData();
                break;
            case 4:
                viewCustomerFeedback();
                break;
            case 5:
                saveSalesData();
                return 0;
            default:
                printf("Invalid option. Please try again.\n");
        }
    }
}

// Function to initialize car data
void initializeCars() {
    // Example car data
    strcpy(cars[carCount].model, "Toyota Corolla");
    cars[carCount].price = 20000;
    cars[carCount].year = 2021;
    cars[carCount].stock = 10;
    carCount++;

    strcpy(cars[carCount].model, "Honda Civic");
    cars[carCount].price = 22000;
    cars[carCount].year = 2020;
    cars[carCount].stock = 8;
    carCount++;

    strcpy(cars[carCount].model, "Ford Focus");
    cars[carCount].price = 21000;
    cars[carCount].year = 2019;
    cars[carCount].stock = 5;
    carCount++;

    strcpy(cars[carCount].model, "BMW 3 Series");
    cars[carCount].price = 35000;
    cars[carCount].year = 2018;
    cars[carCount].stock = 4;
    carCount++;
}

// Function to view car stock
void viewCarsStock() {
    sortCarsByYear();  // Sort cars by year in descending order

    printf("\n%-20s %-10s %-10s %-10s\n", "Model", "Price", "Year", "Stock");
    for (int i = 0; i < carCount; i++) {
        printf("%-20s £%-9.2f %-10d %-10d\n", cars[i].model, cars[i].price, cars[i].year, cars[i].stock);
    }
}

// Function to handle car purchase
void buyCars() {
    char customerName[MAX_NAME_LENGTH];
    getStringInput("Enter customer name: ", customerName, MAX_NAME_LENGTH);
    
    int customerAge = getIntInput("Enter customer age: ");
    if (customerAge < 18) {
        printf("Sorry, customers under the age of 18 are not allowed to buy cars.\n");
        return;
    }

    double totalPrice = 0;
    double discount = 0;
    int purchaseCount = 0;
    char purchaseModels[MAX_CARS][MAX_NAME_LENGTH];
    int purchaseQuantities[MAX_CARS];
    double purchasePrices[MAX_CARS];

    while (1) {
        char model[MAX_NAME_LENGTH];
        getStringInput("Enter car model to buy (or 'done' to finish): ", model, MAX_NAME_LENGTH);
        if (strcmp(model, "done") == 0) break;
        
        int carIndex = findCarIndex(model);
        if (carIndex == -1) {
            printf("Car model not found. Please try again.\n");
            continue;
        }
        
        int quantity = getIntInput("Enter quantity: ");
        if (quantity > cars[carIndex].stock) {
            printf("Not enough stock. Please try again.\n");
            continue;
        }
        
        // Update car stock and calculate price
        cars[carIndex].stock -= quantity;
        double price = cars[carIndex].price * quantity;
        totalPrice += price;

        strcpy(purchaseModels[purchaseCount], model);
        purchaseQuantities[purchaseCount] = quantity;
        purchasePrices[purchaseCount] = price;
        purchaseCount++;
    }

    // Apply discount for senior customers
    if (customerAge > 60) {
        discount = totalPrice * 0.10;
        totalPrice -= discount;
    }

    // Get payment method
    int paymentMethod = getIntInput("Choose payment method: 1 for upfront, 2 for finance: ");
    double totalFinanceAmount = 0;
    double monthlyPayment = 0;
    int financeDuration = 0;

    if (paymentMethod == 2) {
        financeDuration = getIntInput("Enter finance duration in months: ");
        double annualInterestRate = getDoubleInput("Enter annual interest rate (e.g., 5 for 5%): ");
        totalFinanceAmount = totalPrice + (totalPrice * (annualInterestRate / 100) * (financeDuration / 12.0));
        monthlyPayment = calculateMonthlyPayment(totalPrice, annualInterestRate, financeDuration);
        printf("Finance Details: Total Amount = £%.2f, Monthly Payment = £%.2f for %d months\n", totalFinanceAmount, monthlyPayment, financeDuration);
    }

    // Get current date
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char date[MAX_NAME_LENGTH];
    snprintf(date, MAX_NAME_LENGTH, "%02d-%02d-%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);

    // Record sale details
    Sale sale = {0};
    strcpy(sale.customerName, customerName);
    sale.customerAge = customerAge;
    sale.totalPrice = totalPrice;
    sale.discount = discount;
    sale.purchaseCount = purchaseCount;
    strcpy(sale.date, date);
    sale.financeDuration = financeDuration;
    sale.monthlyPayment = monthlyPayment;
    sale.totalFinanceAmount = totalFinanceAmount;
    for (int i = 0; i < purchaseCount; i++) {
        strcpy(sale.purchaseModels[i], purchaseModels[i]);
        sale.purchaseQuantities[i] = purchaseQuantities[i];
        sale.purchasePrices[i] = purchasePrices[i];
    }
    sales[saleCount++] = sale;

    printf("Purchase successful!\n");
    printf("Total price: £%.2f (Discount: £%.2f)\n", totalPrice, discount);

    if (paymentMethod == 2) {
        printf("Finance Details: Total Amount = £%.2f, Monthly Payment = £%.2f for %d months\n", totalFinanceAmount, monthlyPayment, financeDuration);
    }

    // Collect customer feedback
    int rating = getIntInput("Enter feedback rating (1-5): ");
    if (rating < 1 || rating > 5) {
        printf("Invalid rating. Skipping feedback.\n");
    } else {
        char comment[MAX_FEEDBACK_LENGTH];
        getStringInput("Enter feedback comment: ", comment, MAX_FEEDBACK_LENGTH);
        Feedback fb = {0};
        strcpy(fb.model, purchaseModels[0]);
        fb.rating = rating;
        strcpy(fb.comment, comment);
        feedbacks[feedbackCount++] = fb;
        printf("Thank you for your feedback!\n");
    }
}

// Function to view sales data
void viewSalesData() {
    printf("\n%-20s %-10s %-10s %-10s %-10s\n", "Customer Name", "Age", "Total Price", "Discount", "Date");
    for (int i = 0; i < saleCount; i++) {
        printf("%-20s %-10d £%-9.2f £%-9.2f %-10s\n", sales[i].customerName, sales[i].customerAge, sales[i].totalPrice, sales[i].discount, sales[i].date);
        if (sales[i].financeDuration > 0) {
            printf("Finance: Duration = %d months, Monthly Payment = £%.2f, Total Amount = £%.2f\n", sales[i].financeDuration, sales[i].monthlyPayment, sales[i].totalFinanceAmount);
        }
    }
}

// Function to view customer feedback
void viewCustomerFeedback() {
    printf("\n%-20s %-10s %-10s\n", "Model", "Rating", "Comment");
    for (int i = 0; i < feedbackCount; i++) {
        printf("%-20s %-10d %-10s\n", feedbacks[i].model, feedbacks[i].rating, feedbacks[i].comment);
    }
}

// Function to sort cars by year in descending order
void sortCarsByYear() {
    for (int i = 0; i < carCount - 1; i++) {
        for (int j = 0; j < carCount - i - 1; j++) {
            if (cars[j].year < cars[j + 1].year) {
                swapCars(j, j + 1);
            }
        }
    }
}

// Function to find the index of a car model in the inventory
int findCarIndex(const char *model) {
    for (int i = 0; i < carCount; i++) {
        if (strcmp(cars[i].model, model) == 0) {
            return i;
        }
    }
    return -1;
}

// Function to swap two cars in the array
void swapCars(int i, int j) {
    Car temp = cars[i];
    cars[i] = cars[j];
    cars[j] = temp;
}

// Function to get integer input from the user
int getIntInput(const char *prompt) {
    int input;
    printf("%s", prompt);
    scanf("%d", &input);
    while (getchar() != '\n');  // Clear the input buffer
    return input;
}

// Function to get double input from the user
double getDoubleInput(const char *prompt) {
    double input;
    printf("%s", prompt);
    scanf("%lf", &input);
    while (getchar() != '\n');  // Clear the input buffer
    return input;
}

// Function to get string input from the user
void getStringInput(const char *prompt, char *input, int maxLength) {
    printf("%s", prompt);
    fgets(input, maxLength, stdin);
    input[strcspn(input, "\n")] = '\0';  // Remove trailing newline
}

// Function to display sales summary by model
void displaySalesSummary() {
    printf("\n--- Sales Summary by Model ---\n");

    // Calculate total sales by model
    double salesByModel[MAX_CARS] = {0};
    int quantitiesByModel[MAX_CARS] = {0};

    for (int i = 0; i < saleCount; i++) {
        for (int j = 0; j < sales[i].purchaseCount; j++) {
            int carIndex = findCarIndex(sales[i].purchaseModels[j]);
            if (carIndex != -1) {
                salesByModel[carIndex] += sales[i].purchasePrices[j];
                quantitiesByModel[carIndex] += sales[i].purchaseQuantities[j];
            }
        }
    }

    // Display summary
    printf("%-20s %-10s %-10s\n", "Model", "Total Sales", "Total Quantity");
    for (int i = 0; i < carCount; i++) {
        printf("%-20s £%-10.2f %-10d\n", cars[i].model, salesByModel[i], quantitiesByModel[i]);
    }
}

// Function to load sales data from file
void loadSalesData() {
    FILE *file = fopen(FILENAME, "rb");
    if (file != NULL) {
        fread(&saleCount, sizeof(int), 1, file);
        fread(sales, sizeof(Sale), saleCount, file);
        fclose(file);
    }
}

// Function to save sales data to file
void saveSalesData() {
    FILE *file = fopen(FILENAME, "wb");
    if (file != NULL) {
        fwrite(&saleCount, sizeof(int), 1, file);
        fwrite(sales, sizeof(Sale), saleCount, file);
        fclose(file);
    }
}

// Function to calculate monthly payment for finance
double calculateMonthlyPayment(double amount, double annualRate, int months) {
    double monthlyRate = annualRate / 12 / 100;
    return (amount * monthlyRate) / (1 - pow(1 + monthlyRate, -months));
}
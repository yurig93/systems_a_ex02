//
// Created by Yuri Grigorian on 30/11/2020.
//

#include "myBank.h"
#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>


void print_usage() {
    printf("Please choose a transaction type:\n"
           " O-Open Account\n"
           " B-Balance Inquiry\n"
           " D-Deposit\n"
           " W-Withdrawal\n"
           " C-Close Account\n"
           " I-Interest\n"
           " P-Print\n"
           " E-Exit\n");
    fflush(stdout);
}

///
/// \param iAccountIdx Account index.
/// \return Account number.
int convert_idx_to_account_number(int iAccountIdx) {
    return ACCOUNT_NUMBER_START + iAccountIdx;
}

///
/// \param iAccountNum Account number.
/// \return Account index.
int convert_account_number_to_idx(int iAccountNum) {
    return iAccountNum - ACCOUNT_NUMBER_START;
}

///
/// \param iAccountNum
/// \return STATUS_RUNTIME_OK on success. STATUS_RUNTIME_ERR_BAD_RANGE when account is invalid.
int validate_account_number(int iAccountNum) {
    return (iAccountNum >= MIN_ACCOUNT_NUMBER && iAccountNum < MAX_ACCOUNT_NUMBER) ? STATUS_RUNTIME_OK
                                                                                   : STATUS_RUNTIME_ERR_BAD_RANGE;
}
/// Calculates the memory address for an account and account option.
/// \param dpAccounts Pointer to accounts array.
/// \param iAccountIdx Account index.
/// \param iOption Account option (balance or status usually);
/// \return Address of memory.
double *get_account_option_addr(const double *dpAccounts, int iAccountIdx, int iOption) {
    // No boundary checks here, just calculation.
    return (double *) (dpAccounts + iAccountIdx * 2 + iOption);
}

int validate_account_number_open(const double *dpAccounts, int iAccountNum) {
    int iOk = validate_account_number(iAccountNum);
    if (iOk != STATUS_RUNTIME_OK) {
        return iOk;
    }
    int iAccountIdx = convert_account_number_to_idx(iAccountNum);
    double *ipAccountStatus = get_account_option_addr(dpAccounts, iAccountIdx, OPT_ACC_STATUS);

    return *(ipAccountStatus) == STATUS_ACC_OPEN ? STATUS_RUNTIME_OK
                                                 : STATUS_RUNTIME_ERR_ACCOUNT_CLOSED;
}

/// Gets a double from user.
/// \param ipStatus Status pointer. Pointer value set according to status.
/// \param bVerbose Should print.
/// \return Entered double.
double get_user_input_double(int *ipStatus, bool bVerbose) {
    char in[DEFAULT_BUFFER_INPUT_CHAR];
    fgets(in, sizeof(in), stdin);
    char *pInEnd;
    double amount = strtod(in, &pInEnd);
    *ipStatus = (pInEnd == in || errno == ERANGE) ? STATUS_RUNTIME_ERR_FAILED_TO_READ_DOUBLE : STATUS_RUNTIME_OK;

    if (bVerbose) {
        if (*ipStatus != STATUS_RUNTIME_OK) {
            printf("Failed to read the amount");
        }
    }
    return amount;
}

/// Gets a positive double from user.
/// \param ipStatus Status pointer. Pointer value set according to status.
/// \param bVerbose Should print.
/// \return Entered double.
double get_user_input_positive_amount(int *ipStatus, bool bVerbose) {
    double amount = get_user_input_double(ipStatus, bVerbose);

    if (amount < 0) {
        *ipStatus = STATUS_RUNTIME_ERR_NEGATIVE_INPUT;
    }

    if (bVerbose) {
        switch (*ipStatus) {
            case STATUS_RUNTIME_ERR_NEGATIVE_INPUT:
                printf("Cannot get a negative amount");
                break;
        }
    }
    return amount;
}

/// Gets account number from user and validates its correct.
/// \param ipStatus Status pointer.
/// \param bVerbose Should print.
/// \return Account num.
int get_user_account_number(int *ipStatus, bool bVerbose) {
    if (bVerbose) {
        printf("Please enter account number: ");
    }

    int number = (int) get_user_input_double(ipStatus, false);

    if (*ipStatus == STATUS_RUNTIME_OK) {
        *ipStatus = validate_account_number(number);
    }

    if (bVerbose) {
        switch (*ipStatus) {
            case STATUS_RUNTIME_ERR_BAD_RANGE:
                printf("Invalid account number");
                break;
            case STATUS_RUNTIME_ERR_FAILED_TO_READ_DOUBLE:
                printf("Failed to read the account number");
                break;
        }
    }

    return number;
}

/// Gets account number from user and validates its correct and open.
/// \param dpAccounts Accounts pointer.
/// \param ipStatus Status pointer.
/// \param bVerbose Should print.
/// \return Account number.
int get_user_account_number_open(double *dpAccounts, int *ipStatus, bool bVerbose) {
    *ipStatus = STATUS_RUNTIME_OK;
    int iAccountNum = get_user_account_number(ipStatus, bVerbose);

    if (*ipStatus != STATUS_RUNTIME_OK) {
        return 0;
    }

    *ipStatus = validate_account_number_open(dpAccounts, iAccountNum);
    if (*ipStatus == STATUS_RUNTIME_ERR_ACCOUNT_CLOSED) {
        printf("This account is closed");
        return 0;
    }

    return iAccountNum;
}

/// Find a free index for a new account.
/// \param dpAccounts Accounts pointer.
/// \param iAccountsLen Account len.
/// \return Account index or -1 if none found.
int get_next_closed_account_idx(double *dpAccounts, int iAccountsLen) {
    // O(n) can be optimized to keep idx of last closed account.
    for (int i = 0; i < iAccountsLen; ++i) {
        if (*get_account_option_addr(dpAccounts, i, OPT_ACC_STATUS) == STATUS_ACC_CLOSED) {
            return i;
        }
    }
    return -1;
}

/// Deposits an amount to an account.
/// \param dpAccounts Account pointer.
/// \param iAccountIdx Account index.
/// \param ipStatus Status pointer.
/// \param bPrintNewBalance Should print new balance.
/// \return Deposited amount.
double account_deposit(double *dpAccounts, int iAccountIdx, int *ipStatus, bool bPrintNewBalance) {
    printf("Please enter amount for deposit: ");
    double amount = get_user_input_positive_amount(ipStatus, false);
    double *dpAccountBalance = get_account_option_addr(dpAccounts, iAccountIdx, OPT_ACC_BALANCE);

    switch (*ipStatus) {
        case STATUS_RUNTIME_OK:
            *dpAccountBalance += amount;
            if (bPrintNewBalance) {
                printf("The new balance is: %.2f", *dpAccountBalance);
            }
            break;
        case STATUS_RUNTIME_ERR_FAILED_TO_READ_DOUBLE:
            printf("Failed to read the amount");
            break;
        case STATUS_RUNTIME_ERR_NEGATIVE_INPUT:
            printf("Cannot deposit a negative amount");
            break;
    }
    return amount;
}

/// Withdrawals as amount from an account.
/// \param dpAccounts Accounts pointer.
/// \param iAccountIdx Accounts index.
/// \param ipStatus Status pointer.
/// \return Amount withdrawn.
double account_withdrawal(double *dpAccounts, int iAccountIdx, int *ipStatus) {
    printf("Please enter the amount to withdraw: ");
    double amount = get_user_input_positive_amount(ipStatus, false);
    double *dpAccountBalance = get_account_option_addr(dpAccounts, iAccountIdx, OPT_ACC_BALANCE);

    if (*ipStatus == STATUS_RUNTIME_OK) {
        if (amount > *dpAccountBalance) {
            *ipStatus = STATUS_RUNTIME_ERR_NO_FUNDS;
        }
    }

    switch (*ipStatus) {
        case STATUS_RUNTIME_OK:
            *dpAccountBalance -= amount;
            printf("The new balance is: %.2f", *dpAccountBalance);
            break;
        case STATUS_RUNTIME_ERR_FAILED_TO_READ_DOUBLE:
            printf("Failed to read the amount");
            break;
        case STATUS_RUNTIME_ERR_NEGATIVE_INPUT:
            printf("Cannot withdraw a negative amount");
            break;
        case STATUS_RUNTIME_ERR_NO_FUNDS:
            printf("Cannot withdraw more than the balance");
            break;
    }

    return amount;
}

/// Applies intereset to all open account,
/// \param dpAccounts Account pointer.
/// \param iAccountsLen Accounts len.
/// \param dInterestPercent Interest as percent.
/// \return Number of applied accounts.
int accounts_interest(double *dpAccounts, int iAccountsLen, double dInterestPercent) {
    int iAppliedAccounts = 0;

    for (int i = 0; i < iAccountsLen; ++i) {
        double *dpAccountsStatus = get_account_option_addr(dpAccounts, i, OPT_ACC_STATUS);

        if (*dpAccountsStatus == STATUS_ACC_OPEN) {
            double *dpAccountsBalance = get_account_option_addr(dpAccounts, i, OPT_ACC_BALANCE);
            *dpAccountsBalance += *dpAccountsBalance * (dInterestPercent / 100);
            ++iAppliedAccounts;
        }
    }
    return iAppliedAccounts;
}

/// Prints account balance.
/// \param dpAccounts Accounts pointer.
/// \param iAccountNum Account number.
void account_balance(double *dpAccounts, int iAccountNum) {
    double *ipAccountBalance = get_account_option_addr(dpAccounts, convert_account_number_to_idx(iAccountNum),
                                                       OPT_ACC_BALANCE);
    printf("The balance of account number %d is: %.2f", iAccountNum, *ipAccountBalance);
}


/// Closes an account.
/// \param dpAccounts Accounts pointer.
/// \param iAccountNum Account number.
void account_close(double *dpAccounts, int iAccountNum) {
    double *ipAccountStatus = get_account_option_addr(dpAccounts, convert_account_number_to_idx(iAccountNum),
                                                      OPT_ACC_STATUS);
    *ipAccountStatus = STATUS_ACC_CLOSED;
}

/// Prints open account balances.
/// \param dpAccounts Accounts pointer.
/// \param iAccountsLen Accounts len.
void accounts_print(double *dpAccounts, int iAccountsLen) {
    for (int i = 0; i < iAccountsLen; ++i) {
        double *dpAccountsStatus = get_account_option_addr(dpAccounts, i, OPT_ACC_STATUS);
        if (*dpAccountsStatus == STATUS_ACC_OPEN) {
            account_balance(dpAccounts, convert_idx_to_account_number(i));
        }
    }
}

/// Menu entry for account opening.
/// \param dpAccounts Accounts pointer.
/// \param iAccountsLen Accounts len.
/// \return The index of the new account.
int menu_entry_account_open(double *dpAccounts, int iAccountsLen) {
    int iOk = 0;
    int iFreeAccountIdx = get_next_closed_account_idx(dpAccounts, iAccountsLen);
    if (iFreeAccountIdx < 0) {
        printf("There are no free accounts to use");
        return STATUS_RUNTIME_ERR_NO_FREE_ACCOUNT;
    }

    account_deposit(dpAccounts, iFreeAccountIdx, &iOk, false);

    double *ipAccountStatus = get_account_option_addr(dpAccounts, iFreeAccountIdx, OPT_ACC_STATUS);
    *ipAccountStatus = STATUS_ACC_OPEN;

    if (iOk == STATUS_RUNTIME_OK) {
        printf("New account number is: %d", convert_idx_to_account_number(iFreeAccountIdx));
    }
    return iFreeAccountIdx;
}

/// Menu entry for account deposit.
/// \param dpAccounts Accounts pointer.
void menu_entry_account_deposit(double *dpAccounts) {
    int iOk = STATUS_RUNTIME_OK;
    int iAccountNum = get_user_account_number_open(dpAccounts, &iOk, true);
    if (iOk == STATUS_RUNTIME_OK) {
        account_deposit(dpAccounts, convert_account_number_to_idx(iAccountNum), &iOk, true);
    }
}

/// Menu entry for account balance.
/// \param dpAccounts Accounts pointer.
void menu_entry_account_balance(double *dpAccounts) {
    int iOk = STATUS_RUNTIME_OK;
    int iAccountNum = get_user_account_number_open(dpAccounts, &iOk, true);

    if (iOk == STATUS_RUNTIME_OK) {
        account_balance(dpAccounts, iAccountNum);
    }
}

/// Menu entry for account withdrawal.
/// \param dpAccounts Accounts pointer.
void menu_entry_account_withdrawal(double *dpAccounts) {
    int iOk = STATUS_RUNTIME_OK;
    int iAccountNum = get_user_account_number_open(dpAccounts, &iOk, true);

    if (iOk == STATUS_RUNTIME_OK) {
        account_withdrawal(dpAccounts, convert_account_number_to_idx(iAccountNum), &iOk);
    }
}

/// Menu entry for interest apply.
/// \param dpAccounts Accounts pointer.
/// \param iAccountsLen Accounts len.
void menu_entry_accounts_interest(double *dpAccounts, int iAccountsLen) {
    printf("Please enter interest rate: ");
    int iOk = STATUS_RUNTIME_OK;
    double percent = get_user_input_positive_amount(&iOk, false);

    switch (iOk) {
        case STATUS_RUNTIME_OK:
            accounts_interest(dpAccounts, iAccountsLen, percent);
            break;
        case STATUS_RUNTIME_ERR_FAILED_TO_READ_DOUBLE:
            printf("Failed to read the interest rate");
            break;
        case STATUS_RUNTIME_ERR_NEGATIVE_INPUT:
            printf("Invalid interest rate");
            break;
    }
}

/// Menu entry for accounts balance print.
/// \param dpAccounts Accounts pointer.
/// \param iAccountsLen Accounts len.
void menu_entry_accounts_print(double *dpAccounts, int iAccountsLen) {
    accounts_print(dpAccounts, iAccountsLen);
}

/// Menu entry for account close.
/// \param dpAccounts Accounts pointer.
void menu_entry_account_close(double *dpAccounts) {
    int iOk = STATUS_RUNTIME_OK;
    int iAccountNum = get_user_account_number(&iOk, true);

    if (iOk == STATUS_RUNTIME_OK) {
        iOk = validate_account_number_open(dpAccounts, iAccountNum);
        if (iOk == STATUS_RUNTIME_ERR_ACCOUNT_CLOSED) {
            printf("This account is already closed");
            return;
        }
        account_close(dpAccounts, iAccountNum);
        printf("Closed account number %d", iAccountNum);
    }
}

/// Menu entry for closing all accounts.
/// \param dpAccounts Accounts pointer.
/// \param iAccountsLen Accounts len.
void menu_entry_accounts_close_all(double *dpAccounts, int iAccountsLen) {
    for (int i = 0; i < iAccountsLen; ++i) {
        account_close(dpAccounts, convert_idx_to_account_number(i));
    }
}

/// Main menu loop.
/// \param dpAccounts
/// \param iAccountsLen
void loop(double *dpAccounts, int iAccountsLen) {
    char in[DEFAULT_BUFFER_INPUT_CHAR];

    while (1) {
        printf("\n");
        print_usage();
        fgets(in, sizeof(in), stdin);

        if (!strcasecmp(in, "E\n")) {
            menu_entry_accounts_close_all(dpAccounts, iAccountsLen);
            return;
        } else if (!strcasecmp(in, "O\n")) {
            menu_entry_account_open(dpAccounts, iAccountsLen);
        } else if (!strcasecmp(in, "B\n")) {
            menu_entry_account_balance(dpAccounts);
        } else if (!strcasecmp(in, "D\n")) {
            menu_entry_account_deposit(dpAccounts);
        } else if (!strcasecmp(in, "W\n")) {
            menu_entry_account_withdrawal(dpAccounts);
        } else if (!strcasecmp(in, "I\n")) {
            menu_entry_accounts_interest(dpAccounts, iAccountsLen);
        } else if (!strcasecmp(in, "C\n")) {
            menu_entry_account_close(dpAccounts);
        } else if (!strcasecmp(in, "P\n")) {
            menu_entry_accounts_print(dpAccounts, iAccountsLen);
        } else {
            printf("Invalid transaction type");
        }
        printf("\n");
    }
}
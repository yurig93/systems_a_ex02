//
// Created by Yuri Grigorian on 30/11/2020.
//

#ifndef SYSTEMS_A_EX02_MYBANK_H
#define SYSTEMS_A_EX02_MYBANK_H
#include <stdbool.h>

#define DEFAULT_BUFFER_INPUT_CHAR 256
#define NUM_ACCOUNTS 50
#define ACCOUNT_NUMBER_START 901
#define MIN_ACCOUNT_NUMBER ACCOUNT_NUMBER_START
#define MAX_ACCOUNT_NUMBER (MIN_ACCOUNT_NUMBER+NUM_ACCOUNTS)

enum eAccountOptions {
    OPT_ACC_STATUS = 0,
    OPT_ACC_BALANCE,
};

enum eAccountStatuses {
    STATUS_ACC_CLOSED = 0,
    STATUS_ACC_OPEN,
};

// TODO: next exercise optimize and glue error codes to error msgs :)
enum eErrors {
    STATUS_RUNTIME_OK = 0,
    STATUS_RUNTIME_ERR_GENERAL,
    STATUS_RUNTIME_ERR_FAILED_TO_READ_DOUBLE,
    STATUS_RUNTIME_ERR_NO_FREE_ACCOUNT,
    STATUS_RUNTIME_ERR_NEGATIVE_INPUT,
    STATUS_RUNTIME_ERR_BAD_RANGE,
    STATUS_RUNTIME_ERR_ACCOUNT_CLOSED,
    STATUS_RUNTIME_ERR_NO_FUNDS,
};

static const int SI_ACCOUNT_OPTIONS_LEN = OPT_ACC_BALANCE - OPT_ACC_STATUS + 1; // 2

void print_usage();

int convert_idx_to_account_number(int iAccountIdx);

int convert_account_number_to_idx(int iAccountNum);

int validate_account_number(int iAccountNum);

double *get_account_option_addr(const double *dpAccounts, int iAccountIdx, int iOption);

int validate_account_number_open(const double *dpAccounts, int iAccountNum);

double get_user_input_double(int *ipStatus, bool bVerbose);

double get_user_input_positive_amount(int *ipStatus, bool bVerbose);

int get_user_account_number(int *ipStatus, bool bVerbose);

int get_user_account_number_open(double *dpAccounts, int *ipStatus, bool bVerbose);

int get_next_closed_account_idx(double *dpAccounts, int iAccountsLen);

double account_deposit(double *dpAccounts, int iAccountIdx, int *ipStatus, bool bPrintNewBalance);

double account_withdrawal(double *dpAccounts, int iAccountIdx, int *ipStatus);

int accounts_interest(double *dpAccounts, int iAccountsLen, double dInterestPercent);

void account_balance(double *dpAccounts, int iAccountNum);

void account_close(double *dpAccounts, int iAccountNum);

void accounts_print(double *dpAccounts, int iAccountsLen);

int menu_entry_account_open(double *dpAccounts, int iAccountsLen);

void menu_entry_account_deposit(double *dpAccounts);

void menu_entry_account_balance(double *dpAccounts);

void menu_entry_account_withdrawal(double *dpAccounts);

void menu_entry_accounts_interest(double *dpAccounts, int iAccountsLen);

void menu_entry_accounts_print(double *dpAccounts, int iAccountsLen);

void menu_entry_account_close(double *dpAccounts);

void menu_entry_accounts_close_all(double *dpAccounts, int iAccountsLen);

void loop(double *dpAccounts, int iAccountsLen);

#endif //SYSTEMS_A_EX02_MYBANK_H

#include "myBank.h"
int main() {
    const int LEN_ACCOUNTS = NUM_ACCOUNTS;
    double accounts[LEN_ACCOUNTS][SI_ACCOUNT_OPTIONS_LEN] = {{0}};
    loop((double *)accounts, LEN_ACCOUNTS);
}
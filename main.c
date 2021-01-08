#include "myBank.h"


int main() {
    const int LEN_ACCOUNTS = NUM_ACCOUNTS;
    double accounts[LEN_ACCOUNTS][SI_ACCOUNT_OPTIONS_LEN];

    // Zero out arr, memset possible too.
    for (int i = 0; i < LEN_ACCOUNTS; ++i) {
        for (int j = 0; j < SI_ACCOUNT_OPTIONS_LEN; ++j) {
            accounts[i][j] = 0;
        }
    }

    loop((double *) accounts, LEN_ACCOUNTS);
}
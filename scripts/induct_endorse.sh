#!/usr/bin/env sh
set -xe

CLEOS="cleos -u https://waxtest.eosn.io"
CONTRACT="test.edev"
INVITER="edenmember11"
WITNESS1="edenmember12"
WITNESS2="edenmember13"
ADD_TO_ID=100

for PARMS in $(cat ./genesis_accounts_long_list); do
    INDUCTION_ID=$(echo $PARMS | cut -d "," -f2);
    ID=$((INDUCTION_ID+ADD_TO_ID))
    HASH=$(echo $PARMS | cut -d "," -f6);

    INVITER_ACTION_DATA="{
        \"account\": \"$INVITER\",
        \"id\": $ID,
        \"induction_data_hash\": \"$HASH\",
    }"

    $CLEOS push action $CONTRACT inductendors "$INVITER_ACTION_DATA" -p "$INVITER@active"

    WITNESS1_ACTION_DATA="{
        \"account\": \"$WITNESS1\",
        \"id\": $ID,
        \"induction_data_hash\": \"$HASH\",
    }"

    $CLEOS push action $CONTRACT inductendors "$WITNESS1_ACTION_DATA" -p "$WITNESS1@active"

    WITNESS2_ACTION_DATA="{
        \"account\": \"$WITNESS2\",
        \"id\": $ID,
        \"induction_data_hash\": \"$HASH\",
    }"

    $CLEOS push action $CONTRACT inductendors "$WITNESS2_ACTION_DATA" -p "$WITNESS2@active"

done;

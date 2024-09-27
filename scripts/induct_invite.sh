#!/usr/bin/env sh
set -xe

CLEOS="cleos -u https://waxtest.eosn.io"
CONTRACT="test.edev"
INVITER="edenmember11"
WITNESS1="edenmember12"
WITNESS2="edenmember13"
ADD_TO_ID=100

for PARMS in $(cat ./genesis_accounts_long_list); do
  INVITEE=$(echo $PARMS | cut -d "," -f1);
  INDUCTION_ID=$(echo $PARMS | cut -d "," -f2);
  ID=$((INDUCTION_ID+ADD_TO_ID))

  INVITE_ACTION_DATA="{
    \"id\": $ID,
    \"inviter\": \"$INVITER\",
    \"invitee\": \"$INVITEE\",
    \"witnesses\": [\"$WITNESS1\", \"$WITNESS2\"]
  }"

# echo $INVITE_ACTION_DATA
$CLEOS push action $CONTRACT inductinit "$INVITE_ACTION_DATA" -p "$INVITER@active"

done;

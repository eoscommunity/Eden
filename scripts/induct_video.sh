#!/usr/bin/env sh
set -xe

CLEOS="cleos -u https://waxtest.eosn.io"
CONTRACT="test.edev"
INVITER="edenmember11"
VIDEO_CID="QmTYqoPYf7DiVebTnvwwFdTgsYXg2RnuPrt8uddjfW2kHS"
ADD_TO_ID=100

for PARMS in $(cat ./genesis_accounts_long_list); do
  INDUCTION_ID=$(echo $PARMS | cut -d "," -f2);
  ID=$((INDUCTION_ID+ADD_TO_ID))

  ACTION_DATA="{
    \"account\": \"$INVITER\",
    \"id\": $ID,
    \"video\": \"$VIDEO_CID\"
  }"

# echo $INVITE_ACTION_DATA
$CLEOS push action $CONTRACT inductvideo "$ACTION_DATA" -p "$INVITER@active"

done;

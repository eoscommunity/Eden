#!/usr/bin/env sh
set -xe

CLEOS="cleos -u https://waxtest.eosn.io"
CONTRACT="test.edev"
ADD_TO_ID=100

for PARMS in $(cat ./genesis_accounts_long_list); do
  ACCOUNT_NAME=$(echo $PARMS | cut -d "," -f1);
  INDUCTION_ID=$(echo $PARMS | cut -d "," -f2);
  ID=$((INDUCTION_ID+ADD_TO_ID))
  IMAGE=$(echo $PARMS | cut -d "," -f3);
  FNAME=$(echo $PARMS | cut -d "," -f4);
  LNAME=$(echo $PARMS | cut -d "," -f5);
  BIO="Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras sagittis, nulla laoreet vulputate pharetra, eros ex finibus urna, ut semper neque eros eget turpis. Proin semper fringilla lobortis. Phasellus quis sapien nec dolor porta feugiat in eget augue. Etiam porta dolor sed leo tristique mattis sollicitudin sed neque. Proin in iaculis nunc. Integer placerat suscipit malesuada. Nunc pretium in urna ac gravida. Nulla in velit dignissim, bibendum est ut, auctor enim. Vestibulum non aliquet turpis. Etiam mattis, urna id scelerisque aliquet, nibh turpis pulvinar odio, sed tincidunt nisi libero et sem. Integer ultrices, nulla a sollicitudin efficitur, turpis ipsum semper ipsum, et efficitur leo est ut leo. Suspendisse massa ex, venenatis pretium nulla ut, sodales sagittis lectus."

  INVITEE=$ACCOUNT_NAME
  ATTRIBUTIONS="pexels.com"
  SOCIAL_EOSCOMMUNITY=$ACCOUNT_NAME
  SOCIAL_TWITTER=$ACCOUNT_NAME
  SOCIAL_LINKEDIN=$ACCOUNT_NAME
  SOCIAL_TELEGRAM=$ACCOUNT_NAME
  SOCIAL_FACEBOOK=$ACCOUNT_NAME
  SOCIAL_BLOG="moreequalanimals.com"

  PROFILE_ACTION_DATA="{
    \"id\": $ID,
    \"new_member_profile\": {
      \"name\": \"$FNAME $LNAME\",
      \"img\": \"$IMAGE\",
      \"bio\": \"$BIO\",
      \"social\": \"{\\\"eosCommunity\\\":\\\"$SOCIAL_EOSCOMMUNITY\\\",\\\"twitter\\\":\\\"$SOCIAL_TWITTER\\\",\\\"linkedin\\\":\\\"$SOCIAL_LINKEDIN\\\",\\\"telegram\\\":\\\"$SOCIAL_TELEGRAM\\\",\\\"facebook\\\":\\\"$SOCIAL_FACEBOOK\\\",\\\"blog\\\":\\\"$SOCIAL_BLOG\\\"}\",
      \"attributions\": \"$ATTRIBUTIONS\"
    }
  }"

$CLEOS push action $CONTRACT inductprofil "$PROFILE_ACTION_DATA" -p "$INVITEE@active"

done;

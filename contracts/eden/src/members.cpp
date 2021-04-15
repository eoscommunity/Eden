#include <members.hpp>

namespace eden
{
   void members::deposit(eosio::name account, const eosio::asset& quantity)
   {
      eosio::check(quantity >= minimum_membership_donation, "insufficient minimum donation");
      if (is_new_member(account))
      {
         create(account);
      }
   }

   void members::check_active_member(eosio::name account)
   {
      auto member = member_tb.get(account.value);
      eosio::check(member.status == member_status::active_member,
                   "inactive member " + account.to_string());
   }

   void members::check_pending_member(eosio::name account)
   {
      auto member = member_tb.get(account.value);
      eosio::check(member.status == member_status::pending_membership,
                   "member " + account.to_string() + " is not pending");
   }

   bool members::is_new_member(eosio::name account) const
   {
      auto itr = member_tb.find(account.value);
      return itr == member_tb.end();
   }

   void members::create(eosio::name account)
   {
      member_tb.emplace(contract, [&](auto& row) {
         row.account = account;
         row.status = member_status::pending_membership;
         row.nft_template_id = 0;
      });
   }
}  // namespace eden

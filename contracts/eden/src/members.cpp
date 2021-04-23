#include <members.hpp>

namespace eden
{
   void members::deposit(eosio::name account, const eosio::asset& quantity)
   {
      eosio::check(quantity >= globals.get().minimum_donation, "insufficient minimum donation");
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
      auto stats = member_stats.get_or_default();
      ++stats.pending_members;
      eosio::check(stats.pending_members != 0, "Integer overflow");
      member_stats.set(stats, contract);
      member_tb.emplace(contract, [&](auto& row) {
         row.account = account;
         row.status = member_status::pending_membership;
         row.nft_template_id = 0;
      });
   }

   void members::set_nft(eosio::name account, int32_t nft_template_id)
   {
      check_pending_member(account);
      const auto& member = member_tb.get(account.value);
      member_tb.modify(member, eosio::same_payer,
                       [&](auto& row) { row.nft_template_id = nft_template_id; });
   }

   void members::set_active(eosio::name account)
   {
      auto stats = member_stats.get();
      eosio::check(stats.pending_members > 0, "Invariant failure: no pending members");
      eosio::check(stats.active_members < max_active_members,
                   "Invariant failure: active members too high");
      --stats.pending_members;
      ++stats.active_members;
      member_stats.set(stats, eosio::same_payer);
      check_pending_member(account);
      const auto& member = member_tb.get(account.value);
      member_tb.modify(member, eosio::same_payer,
                       [&](auto& row) { row.status = member_status::active_member; });
   }

   struct member_stats members::stats() { return member_stats.get(); }

   void members::clear_all()
   {
      auto members_itr = member_tb.lower_bound(0);
      while (members_itr != member_tb.end())
         member_tb.erase(members_itr++);
      member_stats.remove();
   }
}  // namespace eden

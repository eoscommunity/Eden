#pragma once

#include <constants.hpp>
#include <eden-atomicassets.hpp>
#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <inductions.hpp>
#include <string>
#include <vector>

namespace eden
{
   // Ricardian contracts live in eden-ricardian.cpp
   extern const char* withdraw_ricardian;
   extern const char* genesis_ricardian;
   extern const char* clearall_ricardian;
   extern const char* inductinit_ricardian;
   extern const char* inductprofil_ricardian;
   extern const char* inductvideo_ricardian;
   extern const char* inductendorse_ricardian;
   extern const char* inductdonate_ricardian;
   extern const char* inductcancel_ricardian;
   extern const char* gc_ricardian;
   extern const char* peacetreaty_clause;
   extern const char* bylaws_clause;

   class eden : public eosio::contract
   {
     public:
      using contract::contract;

      eden(eosio::name receiver, eosio::name code, eosio::datastream<const char*> ds)
          : contract(receiver, code, ds)
      {
      }

      void notify_transfer(eosio::name from,
                           eosio::name to,
                           const eosio::asset& quantity,
                           std::string memo);

      void withdraw(eosio::name owner, const eosio::asset& quantity);

      void genesis(std::string community,
                   eosio::symbol community_symbol,
                   eosio::asset minimum_donation,
                   std::vector<eosio::name> initial_members,
                   std::string genesis_video,
                   atomicassets::attribute_map collection_attributes,
                   eosio::asset auction_starting_bid,
                   uint32_t auction_duration,
                   eosio::ignore<std::string> memo);

      void clearall();

      void inductinit(uint64_t id,
                      eosio::name inviter,
                      eosio::name invitee,
                      std::vector<eosio::name> witnesses);

      void inductprofil(uint64_t id, new_member_profile new_member_profile);

      void inductvideo(eosio::name account, uint64_t id, std::string video);

      void inductendorse(eosio::name account, uint64_t id, eosio::checksum256 induction_data_hash);

      void inductdonate(eosio::name payer, uint64_t id, const eosio::asset& quantity);

      void inductcancel(eosio::name account, uint64_t id);

      void inducted(eosio::name inductee);

      void gc(uint32_t limit);

      void notify_lognewtempl(int32_t template_id,
                              eosio::name authorized_creator,
                              eosio::name collection_name,
                              eosio::name schema_name,
                              bool transferable,
                              bool burnable,
                              uint32_t max_supply,
                              const atomicassets::attribute_map& immutable_data);

      void notify_logmint(uint64_t asset_id,
                          eosio::name authorized_minter,
                          eosio::name collection_name,
                          eosio::name schema_name,
                          int32_t template_id,
                          eosio::name new_asset_owner,
                          eosio::ignore<atomicassets::attribute_map>,
                          eosio::ignore<atomicassets::attribute_map>,
                          eosio::ignore<std::vector<eosio::asset>>);
   };

   EOSIO_ACTIONS(
       eden,
       "eden.gm"_n,
       action(withdraw, owner, quantity, ricardian_contract(withdraw_ricardian)),
       action(genesis,
              community,
              community_symbol,
              minimum_donation,
              initial_members,
              genesis_video,
              collection_attributes,
              auction_starting_bid,
              auction_duration,
              memo,
              ricardian_contract(genesis_ricardian)),
       action(clearall, ricardian_contract(clearall_ricardian)),
       action(inductinit,
              id,
              inviter,
              invitee,
              witnesses,
              ricardian_contract(inductinit_ricardian)),
       action(inductprofil, id, new_member_profile, ricardian_contract(inductprofil_ricardian)),
       action(inductvideo, account, id, video, ricardian_contract(inductvideo_ricardian)),
       action(inductendorse,
              account,
              id,
              induction_data_hash,
              ricardian_contract(inductendorse_ricardian)),
       action(inductdonate, payer, id, quantity, ricardian_contract(inductdonate_ricardian)),
       action(inductcancel, account, id, ricardian_contract(inductcancel_ricardian)),
       action(inducted, inductee),
       action(gc, limit, ricardian_contract(gc_ricardian)),
       notify(token_contract, transfer),
       notify(atomic_assets_account, lognewtempl),
       notify(atomic_assets_account, logmint))
}  // namespace eden

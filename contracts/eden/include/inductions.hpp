#pragma once

#include <constants.hpp>
#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <string>
#include <utils.hpp>

namespace eden
{
   struct new_member_profile
   {
      std::string name;
      std::string img;
      std::string bio;
      std::string social;
   };
   EOSIO_REFLECT(new_member_profile, name, img, bio, social)

   struct induction
   {
      uint64_t id;
      eosio::name inviter;
      eosio::name invitee;
      uint32_t endorsements;
      eosio::block_timestamp created_at;
      std::string video;
      new_member_profile new_member_profile;

      uint64_t primary_key() const { return id; }
      uint128_t get_invitee_inviter() const { return combine_names(invitee, inviter); }
      uint128_t get_inviter_invitee() const { return combine_names(inviter, invitee); }
      uint64_t get_created_key() const { return uint64_t{created_at.slot}; }
   };
   EOSIO_REFLECT(induction,
                 id,
                 inviter,
                 invitee,
                 endorsements,
                 created_at,
                 video,
                 new_member_profile)

   using induction_table_type = eosio::multi_index<
       "induction"_n,
       induction,
       eosio::indexed_by<
           "byinvitee"_n,
           eosio::const_mem_fun<induction, uint128_t, &induction::get_invitee_inviter>>,
       eosio::indexed_by<
           "byinviter"_n,
           eosio::const_mem_fun<induction, uint128_t, &induction::get_inviter_invitee>>,
       eosio::indexed_by<"bycreated"_n,
                         eosio::const_mem_fun<induction, uint64_t, &induction::get_created_key>>>;

   struct endorsement
   {
      uint64_t id;
      eosio::name inviter;
      eosio::name invitee;
      eosio::name endorser;
      uint64_t induction_id;
      bool endorsed;

      uint64_t primary_key() const { return id; }
      uint128_t get_endorser_key() const { return uint128_t{endorser.value} << 64 | id; }
      uint64_t induction_id_key() const { return induction_id; }
   };
   EOSIO_REFLECT(endorsement, id, inviter, invitee, endorser, induction_id, endorsed)

   using endorsement_table_type = eosio::multi_index<
       "endorsement"_n,
       endorsement,
       eosio::indexed_by<
           "byendorser"_n,
           eosio::const_mem_fun<endorsement, uint128_t, &endorsement::get_endorser_key>>,
       eosio::indexed_by<
           "byinduction"_n,
           eosio::const_mem_fun<endorsement, uint64_t, &endorsement::induction_id_key>>>;

   class inductions
   {
     private:
      eosio::name contract;
      induction_table_type induction_tb;
      endorsement_table_type endorsement_tb;

      void check_new_induction(eosio::name invitee, eosio::name inviter) const;
      void check_valid_induction(const induction& induction) const;
      void validate_profile(const new_member_profile& new_member_profile) const;
      void check_valid_endorsers(eosio::name inviter,
                                 const std::vector<eosio::name>& witnesses) const;
      void reset_endorsements(uint64_t induction_id);

     public:
      inductions(eosio::name contract)
          : contract(contract),
            induction_tb(contract, default_scope),
            endorsement_tb(contract, default_scope)
      {
      }

      const induction& get_induction(uint64_t id) const;

      void initialize_induction(uint64_t id,
                                eosio::name inviter,
                                eosio::name invitee,
                                const std::vector<eosio::name>& witnesses);

      void update_profile(const induction& induction, const new_member_profile& new_member_profile);

      void create_induction(uint64_t id,
                            eosio::name inviter,
                            eosio::name invitee,
                            uint32_t endorsements);

      void create_endorsement(eosio::name inviter,
                              eosio::name invitee,
                              eosio::name endorser,
                              uint64_t induction_id);

      // this method is used only for administrative purposes,
      // it should never be used outside genesis or test environments
      void clear_all();
   };

}  // namespace eden

#pragma once

#include <steem/protocol/asset_symbol.hpp>

namespace steem { namespace chain {
   class nai_generator {
      /** FNV-1a implementation https://en.wikipedia.org/wiki/Fowler-Noll-Vo_hash_function#FNV-1a_hash */
      struct hasher {
         uint32_t operator()( uint8_t byte, uint32_t hash )
         {
            return (byte ^ hash) * prime;
         }
         
         uint32_t operator()( uint32_t four_bytes, uint32_t hash = offset_basis )
         {
            uint8_t* ptr = (uint8_t*) &four_bytes;
            hash = this->operator()(*ptr++, hash);
            hash = this->operator()(*ptr++, hash);
            hash = this->operator()(*ptr++, hash);
            return this->operator()(*ptr,   hash);
         }
         
      private:
         // Recommended by http://isthe.com/chongo/tech/comp/fnv/
         static const uint32_t prime         = 0x01000193; //   16777619
         static const uint32_t offset_basis  = 0x811C9DC5; // 2166136261
      } hash;

   public:
      asset_symbol_type operator()( uint32_t seed )
      {
         asset_symbol_type new_symbol;
         uint32_t nai = ( ( hash( seed ) % ( ( SMT_MAX_NAI + 1 ) - SMT_MIN_NON_RESERVED_NAI ) ) + SMT_MIN_NON_RESERVED_NAI );
         nai &= ~1;

         uint8_t check_digit = asset_symbol_type::damm_checksum_8digit( nai );
         nai = nai * 10 + check_digit;

         uint32_t asset_num = asset_symbol_type::asset_num_from_nai( nai, 0 );
         new_symbol = asset_symbol_type::from_asset_num( asset_num );

         new_symbol.validate();
         FC_ASSERT( new_symbol.space() == asset_symbol_type::smt_nai_space );

         return new_symbol;
      }
   };
}  } // steem::chain

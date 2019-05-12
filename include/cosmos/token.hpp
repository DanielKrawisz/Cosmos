// Copyright (c) 2018-2019 Daniel Krawisz
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef COSMOS_TOKEN
#define COSMOS_TOKEN

#include "cosmos.hpp"

namespace cosmos {
        
    namespace token {
        
        struct separator;
        struct comma;
        struct open_brace;
        struct close_brace;
        struct open_paren;
        struct close_paren;
        struct set;
        struct plus;
        struct times;
        struct concat;
        
        template <operand op> struct operand;
        
        template <> struct operand<cosmos::plus> {
            using token = plus;
        };
        
        template <> struct operand<cosmos::times> {
            using times = times;
        };
        
        template <> struct operand<cosmos::concat> {
            using concat = concat;
        };
        
        template <typename x> struct write {
            write() = delete;
            void operator()(stringstream& ss) const;
        };
         
        template <typename x> struct read {
            read() = delete;
            bool operator()(stringstream& ss) const;
        };
        
        template <> struct write<separator> {
            void operator()(stringstream& ss) const;
        };
         
        template <> struct read<separator> {
            bool operator()(stringstream& ss) const;
        };
        
        template <> struct write<comma> {
            void operator()(stringstream& ss) const;
        };
         
        template <> struct read<comma> {
            bool operator()(stringstream& ss) const;
        };
        
        template <> struct write<open_brace> {
            void operator()(stringstream& ss) const;
        };
         
        template <> struct read<open_brace> {
            bool operator()(stringstream& ss) const;
        };
        
        template <> struct write<close_brace> {
            void operator()(stringstream& ss) const;
        };
         
        template <> struct read<close_brace> {
            bool operator()(stringstream& ss) const;
        };
        
        template <> struct write<open_paren> {
            void operator()(stringstream& ss) const;
        };
         
        template <> struct read<open_paren> {
            bool operator()(stringstream& ss) const;
        };
        
        template <> struct write<close_paren> {
            void operator()(stringstream& ss) const;
        };
         
        template <> struct read<close_paren> {
            bool operator()(stringstream& ss) const;
        };
        
        template <> struct write<set> {
            void operator()(stringstream& ss) const;
        };
         
        template <> struct read<set> {
            bool operator()(stringstream& ss) const;
        };
        
        template <> struct write<plus> {
            void operator()(stringstream& ss) const;
        };
         
        template <> struct read<plus> {
            bool operator()(stringstream& ss) const;
        };
        
        template <> struct write<times> {
            void operator()(stringstream& ss) const;
        };
         
        template <> struct read<times> {
            bool operator()(stringstream& ss) const;
        };
        
        template <> struct write<concat> {
            void operator()(stringstream& ss) const;
        };
         
        template <> struct read<token::concat> {
            bool operator()(stringstream& ss) const;
        };
        
    }
    
}

#endif 


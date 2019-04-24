#ifndef COSMOS_WORKSPACE
#define COSMOS_WORKSPACE

#include "cosmos.hpp"

namespace cosmos {
    
    namespace work {
        
        struct communicable {
            virtual void text(stringstream&) const = 0;
            virtual void json(stringstream&) const = 0;
        };
        
        
        // all items in the workspace have a name, and
        // this is the type that we use to represent names.
        struct name final : public communicable {
            string Name;
            
            void text(stringstream&) const override;
            void json(stringstream&) const override;
        };
        
        struct binary;
        
        struct number;
        struct pubkey;
        struct secret;
        
        // anything that can appear in the
        // workspace is an item. 
        struct item : public communicable {            
            virtual ptr<binary> concatinate(binary&) const {
                throw exception::invalid_operation{};
            };
        };
        
        // items which have a binary representation
        // in the Bitcoin protocol. 
        struct binary : public virtual item {
            virtual void bytes(ostream&) const = 0;
            ptr<binary> concatinate(binary&) const final override;
        };
        
        struct operation;
        
        // the workspace. 
        struct space final : public item {
            map<name, ptr<item>> Contents;
            bool Valid;
            
            bool valid() const {
                return Valid;
            }
            
            space(const space& s) : Contents{s.Contents}, Valid{s.Valid} {}
            space() : Contents{}, Valid{false} {};
            
            space& operator=(const space& s) {
                Contents = s.Contents;
                Valid = s.Valid;
                return *this;
            }
            
            void text(stringstream&) const override;
            void json(stringstream&) const override;
            
        private:
            space set(name, ptr<item>) const;
            
            friend struct operation;
        };
        
        struct script final : public binary, public bitcoin::script {
            void text(stringstream&) const override;
            void json(stringstream&) const override;
            void bytes(ostream&) const override;
        };
        
        struct address final : public binary, public bitcoin::address {
            void text(stringstream&) const override;
            void json(stringstream&) const override;
            void bytes(ostream&) const override;
        };
        
        struct pubkey final : public binary, public bitcoin::pubkey {
            void text(stringstream&) const override;
            void json(stringstream&) const override;
            void bytes(ostream&) const override;
        };
        
        struct secret final : public binary, public bitcoin::secret {
            void text(stringstream&) const override;
            void json(stringstream&) const override;
            void bytes(ostream&) const override;
        };
        
        struct txid final : public binary, public bitcoin::txid {
            void text(stringstream&) const override;
            void json(stringstream&) const override;
            void bytes(ostream&) const override;
        };
        
        struct digest final : public binary, public bitcoin::digest {
            void text(stringstream&) const override;
            void json(stringstream&) const override;
            void bytes(ostream&) const override;
        };
        
        struct transaction final : public binary {
            void text(stringstream&) const override;
            void json(stringstream&) const override;
            void bytes(ostream&) const override;
        };
        
        struct number final : public item {
            void text(stringstream&) const override;
            void json(stringstream&) const override;
        };
        
        struct wallet final : public item {
            void text(stringstream&) const override;
            void json(stringstream&) const override;
        };
        
        struct keysource final : public item {
            void text(stringstream&) const override;
            void json(stringstream&) const override;
        };
        
        struct operation : public communicable {
            struct application;
            
            virtual application apply(const space) const = 0;
            
            struct identity;
            struct sequence;
            struct set;
            struct function;
            struct name;
            struct constant;
            struct concatination;
            struct plus; 
            struct times;
        
            struct error;
            
        };
        
        struct operation::error {
            std::string Message;
            
            static error unrecognized_name(work::name);
            
            error() : Message{} {}
            error(string m) : Message{m} {};
            
            bool operator==(const error e) const {
                return Message == e.Message;
            }
            
            bool operator!=(const error e) const {
                return Message != e.Message;
            }
            
        };
        
        struct operation::application {
            
            // The new space that results from this operation. 
            space Result; 
            
            // The response which is returned to the user.
            ptr<item> Response; 
            
            error Error;
            
            bool valid() const {
                return Result.valid();
            }
            
            bool error() const {
                return Error != operation::error{};
            };
            
            application(application& a) : Result{a.Result}, Response{a.Response}, Error{a.Error} {}
            
            application& operator=(application&& a) {
                Result = a.Result;
                Response = a.Response;
                Error = a.Error;
                a.Response = nullptr;
                return *this;
            }
        
        private: 
            application() : Result{}, Response{nullptr}, Error{} {}
            application(application&& a) : Result{a.Result}, Response{a.Response}, Error{a.Error} {
                a.Response = nullptr;
            }
            
            application(const space s) : Result{s}, Response{nullptr}, Error{} {};
            application(const space s, ptr<item> i) : Result{s}, Response{i}, Error{} {}
            application(const space s, struct error e) : Result{s}, Response{nullptr}, Error{e} {}
            
            friend struct operation;
        };
            
        struct operation::identity final : public operation {
            application apply(const space s) const override {
                return application{s};
            };
            
            void text(stringstream&) const override;
            void json(stringstream&) const override;
        };
        
        struct operation::constant final : public operation {
            ptr<item> Constant;
            application apply(const space s) const override {
                return {s, Constant};
            }
            
            void text(stringstream&) const override;
            void json(stringstream&) const override;
        };
        
        struct operation::sequence final : public operation {
            list<ptr<operation>> Sequence;
            
            application apply(const space s) const override {
                application a{s};
                for (ptr<operation> o : Sequence) {
                    a = o->apply(a.Result);
                    if (a.error()) return a; 
                }
                return a;
            }
            
            void text(stringstream&) const override;
            void json(stringstream&) const override;
        };
        
        struct operation::set final : public operation {
            work::name Name;
            ptr<operation> Predicate;
            
            application apply(const space s) const override {
                application a = Predicate->apply(s);
                a.Result = s.set(Name, a.Response);
                return a;
            }
            
            void text(stringstream&) const override;
            void json(stringstream&) const override;
        };
        
        struct operation::name final : public operation {
            work::name Name;
            application apply(const space s) const override {
                if (s.Contents.contains(Name)) return {s, s.Contents[Name]};
                else return {s, error::unrecognized_name(Name)};
            }
            
            void text(stringstream&) const override;
            void json(stringstream&) const override;
        };
        
        struct operation::concatination final : public operation {
            application apply(const space) const override;
            
            void text(stringstream&) const override;
            void json(stringstream&) const override;
        };
        
        struct operation::plus final : public operation {
            application apply(const space) const override;
            
            void text(stringstream&) const override;
            void json(stringstream&) const override;
        }; 
        
        struct operation::times final : public operation {
            application apply(const space) const override;
            
            void text(stringstream&) const override;
            void json(stringstream&) const override;
        };
        
        struct operation::function final : public operation {
            application apply(const space) const override;
            
            void text(stringstream&) const override;
            void json(stringstream&) const override;
        };
        
    }
    
}

#endif 

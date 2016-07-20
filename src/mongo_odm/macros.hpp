// Copyright 2016 MongoDB Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <mongo_odm/config/prelude.hpp>

#define MONGO_ODM_PASTE_IMPL(s1, s2) s1##s2
#define MONGO_ODM_PASTE(s1, s2) MONGO_ODM_PASTE_IMPL(s1, s2)

// Wrap a field name to create a corresponding NVP
#define MONGO_ODM_NVP(x) mongo_odm::make_nvp(&mongo_odm_wrap_base::x, #x)

// Macro for creating custom field name
#define MONGO_ODM_CUSTOM_NVP(x, name) mongo_odm::make_nvp(&mongo_odm_wrap_base::x, name)

// Creates serialize() function
#define MONGO_ODM_SERIALIZE_KEYS                                                                \
    template <class Archive>                                                                    \
    void serialize(Archive& ar) {                                                               \
        mongo_odm_serialize_recur<Archive, 0,                                                   \
                                  std::tuple_size<decltype(mongo_odm_mapped_fields())>::value>( \
            ar);                                                                                \
    }                                                                                           \
    template <class Archive, size_t I, size_t N>                                                \
    std::enable_if_t<(I < N), void> mongo_odm_serialize_recur(Archive& ar) {                    \
        auto nvp = std::get<I>(mongo_odm_mapped_fields());                                      \
        ar(cereal::make_nvp(nvp.name, this->*(nvp.t)));                                         \
        mongo_odm_serialize_recur<Archive, I + 1, N>(ar);                                       \
    }                                                                                           \
                                                                                                \
    template <class Archive, size_t I, size_t N>                                                \
    std::enable_if_t<(I == N), void> mongo_odm_serialize_recur(Archive&) {                      \
        ;                                                                                       \
    }

// Register members and create serialize() function
#define MONGO_ODM_MAKE_KEYS(Base, ...)                \
    using mongo_odm_wrap_base = Base;                 \
    constexpr static auto mongo_odm_mapped_fields() { \
        return std::make_tuple(__VA_ARGS__);          \
    }                                                 \
    MONGO_ODM_SERIALIZE_KEYS

// If using the mongo_odm::model, then also register _id as a field.
#define MONGO_ODM_MAKE_KEYS_MODEL(Base, ...) \
    MONGO_ODM_MAKE_KEYS(Base, MONGO_ODM_NVP(_id), __VA_ARGS__)

#define MONGO_ODM_KEY(value) mongo_odm::hasCallIfFieldIsPresent<decltype(&value), &value>::call()
// convenience macro for accessing scalar elements of array fields in query builder.
#define MONGO_ODM_ELEM(value) MONGO_ODM_KEY(value).element()

#define MONGO_ODM_KEY_BY_VALUE(value) \
    mongo_odm::hasCallIfFieldIsPresent<decltype(value), value>::call()

// MONGO_ODM_CHILD* macros, autogenerated to 100 levels (maximum BSON depth)
#include <mongo_odm/mongo_odm_child_autogen.hpp>

#define MONGO_ODM_CHILD(type, ...) \
    MONGO_ODM_PASTE(MONGO_ODM_CHILD, MONGO_ODM_PP_NARG(__VA_ARGS__))(type, __VA_ARGS__)

#define MONGO_ODM_CHILD_ELEM(...) MONGO_ODM_CHILD(__VA_ARGS__).element()

#include <mongo_odm/config/postlude.hpp>
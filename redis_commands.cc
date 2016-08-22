/*
 * This file is open source software, licensed to you under the terms
 * of the Apache License, Version 2.0 (the "License").  See the NOTICE file
 * distributed with this work for additional information regarding copyright
 * ownership.  You may not use this file except in compliance with the License.
 *
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 *
 *  Copyright (c) 2006-2010, Peng Jian, pstack@163.com. All rights reserved.
 *
 */
#include "redis_commands.hh"
#include "redis.hh"
#include <sstream>

namespace redis {
std::vector<sstring> redis_commands::_number_str;
std::vector<sstring> redis_commands::_multi_number_str;
std::vector<sstring> redis_commands::_content_number_str;
redis_commands::redis_commands()
{
    init_number_str_array(_number_str, ":");
    init_number_str_array(_multi_number_str, "*");
    init_number_str_array(_content_number_str, "$");
    _dummy = [] (args_collection&, output_stream<char>& out) {
        return out.write("+BAD\r\n");
    };
    // [TEST]
    // ECHO
    regist_handler("ECHO", [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->echo(args).then([this, &out] (sstring message) {
            scattered_message<char> msg;
            this_type::append_item(msg, std::move(message));
            return out.write(std::move(msg));
        });
    });
    // PING 
    regist_handler("PING", [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->echo(args).then([this, &out] (sstring message) {
            return out.write(msg_pong);
        });
    });

    // INCR
    regist_handler(sstring("INCR"), [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->incr(args).then([this, &out] (uint64_t r) {
            scattered_message<char> msg;
            this_type::append_item(msg, r);
            return out.write(std::move(msg));
        });
    });

    // DECR
    regist_handler(sstring("DECR"), [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->decr(args).then([this, &out] (uint64_t r) {
            scattered_message<char> msg;
            this_type::append_item(msg, r);
            return out.write(std::move(msg));
        });
    });

    // INCRBY
    regist_handler(sstring("INCRBY"), [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->incrby(args).then([this, &out] (uint64_t r) {
            scattered_message<char> msg;
            this_type::append_item(msg, r);
            return out.write(std::move(msg));
        });
    });

    // DECRBY
    regist_handler(sstring("DECRBY"), [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->decrby(args).then([this, &out] (uint64_t r) {
            scattered_message<char> msg;
            this_type::append_item(msg, r);
            return out.write(std::move(msg));
        });
    });

    // SET
    regist_handler(sstring("SET"), [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->set(args).then([this, &out] (int r) {
            return out.write(r == 0 ? msg_ok : msg_err);
        });
    });

    // MSET
    regist_handler(sstring("MSET"), [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->mset(args).then([this, &out] (int r) {
            return out.write(r == 0 ? msg_ok : msg_err);
        });
    });

    // GET
    regist_handler("GET", [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->get(args).then([this, &out] (item_ptr it) {
            scattered_message<char> msg;
            this_type::append_item(msg, std::move(it));
            return out.write(std::move(msg));
        });
    });
    // MGET
    regist_handler("MGET", [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->mget(args).then([this, &out] (std::vector<item_ptr> items) {
            scattered_message<char> msg;
            this_type::append_multi_items(msg, std::move(items));
            return out.write(std::move(msg));
        });
    });

    // COMMAND
    regist_handler(sstring("COMMAND"), [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return out.write(msg_ok);
    });

    // DEL
    regist_handler("DEL", [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->del(args).then([this, &out] (int count) {
            scattered_message<char> msg;
            this_type::append_item(msg, std::move(count));
            return out.write(std::move(msg));
        });
    });

    // EXISTS 
    regist_handler("EXISTS", [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->exists(args).then([this, &out] (int count) {
            scattered_message<char> msg;
            this_type::append_item(msg, std::move(count));
            return out.write(std::move(msg));
        });
    });

    // APPEND 
    regist_handler("APPEND", [this] (args_collection& args, output_stream<char>& out) -> future<> {
         return _redis->append(args).then([this, &out] (int count) {
             scattered_message<char> msg;
             this_type::append_item(msg, std::move(count));
             return out.write(std::move(msg));
         });
    });

    // STRLEN 
    regist_handler("STRLEN", [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->strlen(args).then([this, &out] (int count) {
            scattered_message<char> msg;
            this_type::append_item(msg, std::move(count));
            return out.write(std::move(msg));
        });
    });

    // LPUSH 
    regist_handler("LPUSH", [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->lpush(args).then([this, &out] (int count) {
            scattered_message<char> msg;
            this_type::append_item(msg, std::move(count));
            return out.write(std::move(msg));
        });
    });

    // LPUSHX 
    regist_handler("LPUSHX", [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->lpushx(args).then([this, &out] (int count) {
            scattered_message<char> msg;
            this_type::append_item(msg, std::move(count));
            return out.write(std::move(msg));
        });
    });

    // LPOP 
    regist_handler("LPOP", [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->lpop(args).then([this, &out] (item_ptr item) {
            scattered_message<char> msg;
            this_type::append_item(msg, std::move(item));
            return out.write(std::move(msg));
        });
    });

    // LLEN 
    regist_handler("LLEN", [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->llen(args).then([this, &out] (int count) {
             scattered_message<char> msg;
             this_type::append_item(msg, std::move(count));
             return out.write(std::move(msg));
        });
    });

    // LINDEX 
    regist_handler("LINDEX", [this] (args_collection& args, output_stream<char>& out) -> future<> {
         return _redis->lindex(args).then([this, &out] (item_ptr item) {
             scattered_message<char> msg;
             this_type::append_item(msg, std::move(item));
             return out.write(std::move(msg));
         });
    });

    // LINSERT 
    regist_handler("LINSERT", [this] (args_collection& args, output_stream<char>& out) -> future<> {
         return _redis->linsert(args).then([this, &out] (int count) {
             scattered_message<char> msg;
             this_type::append_item(msg, std::move(count));
             return out.write(std::move(msg));
         });
    });

    // LRANGE 
    regist_handler("LRANGE", [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->lrange(args).then([this, &out] (std::vector<item_ptr> items) {
            scattered_message<char> msg;
            this_type::append_multi_items(msg, std::move(items));
            return out.write(std::move(msg));
        });
    });

    // LSET 
    regist_handler("LSET", [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->lset(args).then([this, &out] (int count) {
            scattered_message<char> msg;
            this_type::append_item(msg, std::move(count));
            return out.write(std::move(msg));
        });
    });

    // RPUSH 
    regist_handler("RPUSH", [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->rpush(args).then([this, &out] (int count) {
            scattered_message<char> msg;
            this_type::append_item(msg, std::move(count));
            return out.write(std::move(msg));
        });
    });

    // RPUSHX 
    regist_handler("RPUSHX", [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->rpushx(args).then([this, &out] (int count) {
            scattered_message<char> msg;
            this_type::append_item(msg, std::move(count));
            return out.write(std::move(msg));
        });
    });

    // RPOP 
    regist_handler("RPOP", [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->rpop(args).then([this, &out] (item_ptr item) {
            scattered_message<char> msg;
            this_type::append_item(msg, std::move(item));
            return out.write(std::move(msg));
        });
    });

    // LREM
    regist_handler("LREM", [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->lrem(args).then([this, &out] (int count) {
            scattered_message<char> msg;
            this_type::append_item(msg, std::move(count));
            return out.write(std::move(msg));
        });
    });

    // LTRIM
    regist_handler("LTRIM", [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->ltrim(args).then([this, &out] (int count) {
            scattered_message<char> msg;
            this_type::append_item(msg, std::move(count));
            return out.write(std::move(msg));
        });
    });

    //[ HASH APIs]
    // HSET
    regist_handler("HSET", [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->hset(args).then([this, &out] (int count) {
            scattered_message<char> msg;
            this_type::append_item(msg, std::move(count));
            return out.write(std::move(msg));
        });
    });
    // HDEL
    regist_handler("HDEL", [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->hdel(args).then([this, &out] (int count) {
            scattered_message<char> msg;
            this_type::append_item(msg, std::move(count));
            return out.write(std::move(msg));
        });
    });
    // HGET
    regist_handler("HGET", [this] (args_collection& args, output_stream<char>& out) -> future<> {
        return _redis->hget(args).then([this, &out] (item_ptr item) {
            scattered_message<char> msg;
            this_type::append_item(msg, std::move(item));
            return out.write(std::move(msg));
        });
    });
}

}

//
// Created by Ruimin Zeng on 2/21/20.
//

#include "warble_server.h"

#include <sstream>
#include <queue>

WarbleServer::WarbleServer(KeyValueStoreClient &client): kvstore_(client) {}

// register the given username in warble server
std::optional<std::string> WarbleServer::Registeruser(const google::protobuf::Any& payload) {
    RegisteruserRequest request;
    payload.UnpackTo(&request);
    std::string username = request.username();
    std::string usernames = kvstore_.Get(std::vector<std::string>(1, "users"))[0];
    std::stringstream ss(usernames);
    std::string tmp;
    bool exist = false;
    while (getline(ss, tmp, ' ')) {
        if (tmp == username) {
            exist = true;
            break;
        }
    }
    if (!exist) {
        kvstore_.Put("users", usernames + " " + username);
        LOG(INFO) << "successfully register " << username << std::endl;
    } else {
        LOG(WARNING) << username << " is already registered" << std::endl;
    }
    return std::nullopt;
}

// store the warble in warble server, return warble reply message
std::optional<std::string> WarbleServer::PublishWarble(const google::protobuf::Any& payload) {
    WarbleRequest request;
    payload.UnpackTo(&request);

    std::string username = request.username();
    std::string text = request.text();
    std::string parent_id = request.parent_id();

    auto* warble = new Warble();
    warble->set_username(username);
    warble->set_text(text);
    warble->set_parent_id(parent_id);

    auto* timestamp = new Timestamp();
    int64_t us = std::chrono::duration_cast< std::chrono::microseconds >(
            std::chrono::system_clock::now().time_since_epoch()
    ).count();
    timestamp->set_useconds(us);
    timestamp->set_seconds(us / 1000000);

    warble->set_id("warble_id_" + std::to_string(us));
    warble->set_allocated_timestamp(timestamp);

    kvstore_.Put(warble->id() + "_content", warble->SerializeAsString());
    std::string son_ids = kvstore_.Get(std::vector<std::string>(1, parent_id))[0];
    kvstore_.Put(parent_id, son_ids + " " + warble->id());
    WarbleReply reply;
    reply.set_allocated_warble(warble);

    return reply.SerializeAsString();
}

// store to_follow into username's following, store username to to_follow's follower
std::optional<std::string> WarbleServer::Follow(const google::protobuf::Any& payload) {
    FollowRequest request;
    payload.UnpackTo(&request);
    std::string username = request.username();
    std::string to_follow = request.to_follow();
    std::string followings = kvstore_.Get(std::vector<std::string>(1, username + "_following"))[0];
    std::stringstream ss(followings);
    std::string tmp;
    bool exist = false;
    while (getline(ss, tmp, ' ')) {
        if (tmp == to_follow) {
            exist = true;
            break;
        }
    }
    if (!exist) {
        kvstore_.Put(username + "_following", followings + " " + to_follow);
        std::string followers = kvstore_.Get(std::vector<std::string>(1, to_follow + "_follower"))[0];
        kvstore_.Put(to_follow + "_follower", followers + " " + username);
        LOG(INFO) << "successfully follow " << to_follow << std::endl;
    }
    LOG(WARNING) << "follow " << to_follow << "again" << std::endl;
    return std::nullopt;
}

// return the warble thread given id
std::optional<std::string> WarbleServer::Read(const google::protobuf::Any& payload) {
    ReadRequest request;
    payload.UnpackTo(&request);
    std::string root_id = request.warble_id();
    ReadReply reply;
    std::queue<std::string> q;
    q.push(root_id);
    while (!q.empty()) {
        std::string curr_id = q.front();
        q.pop();
        Warble obj;
        obj.ParseFromString(kvstore_.Get(std::vector<std::string>(1, curr_id + "_content"))[0]);
        reply.add_warbles()->CopyFrom(obj);
        std::string son_ids = kvstore_.Get(std::vector<std::string>(1, curr_id))[0];
        if (!son_ids.empty()) {
            std::stringstream ss(son_ids);
            std::string id;
            while (getline(ss, id, ' ')) {
                if (!id.empty()) q.push(id);
            }
        }
    }
    return reply.SerializeAsString();
}

// return username's profile
std::optional<std::string> WarbleServer::Profile(const google::protobuf::Any& payload) {
    ProfileRequest request;
    payload.UnpackTo(&request);
    std::string username = request.username();
    ProfileReply reply;
    std::string followings = kvstore_.Get(std::vector<std::string>(1, username + "_following"))[0];
    std::string followers = kvstore_.Get(std::vector<std::string>(1, username + "_follower"))[0];
    std::stringstream ss(followings);
    std::stringstream ss_followers(followers);
    std::string tmp;
    while (getline(ss, tmp, ' ')) {
        reply.add_following(tmp);
    }
    while (getline(ss_followers, tmp, ' ')) {
        reply.add_followers(tmp);
    }
    return reply.SerializeAsString();
}

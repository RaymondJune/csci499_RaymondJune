//
// Created by Ruimin Zeng on 2/21/20.
//

#include "warble_server.h"

#include <queue>
#include <regex>
#include <sstream>

using std::regex;
using std::smatch;
using std::sregex_iterator;

WarbleServer::WarbleServer(KeyValueStoreClient& client) : kvstore_(client) {}

// register the given username in warble server
std::optional<std::string> WarbleServer::Registeruser(
    const google::protobuf::Any& payload) {
  RegisteruserRequest request;
  payload.UnpackTo(&request);
  std::string username = request.username();
  std::string usernames = kvstore_.Get(std::vector<std::string>(1, "users"))[0];
  std::stringstream ss(usernames);
  if (!Check(ss, username)) {
    kvstore_.Put("users", usernames + " " + username);
    LOG(INFO) << "successfully register " << username << std::endl;
  } else {
    LOG(WARNING) << username << " is already registered" << std::endl;
    return username + " is already registered";
  }
  return std::nullopt;
}

// store the warble in warble server, return warble reply message
std::optional<std::string> WarbleServer::PublishWarble(
    const google::protobuf::Any& payload) {
  WarbleRequest request;
  payload.UnpackTo(&request);

  std::string username = request.username();

  if (!ValidateUser(username)) {
    LOG(ERROR) << "the user " << username << "is not registered" << std::endl;
    return "the user is not registered";
  }

  std::string text = request.text();
  std::string parent_id = request.parent_id();

  if (parent_id != "warble_id_root") {
    std::string content =
        kvstore_.Get(std::vector<std::string>(1, parent_id + "_content"))[0];
    if (content.empty()) {
      return "the reply id is invalid";
    }
  }

  auto* warble = new Warble();
  warble->set_username(username);
  warble->set_text(text);
  warble->set_parent_id(parent_id);

  auto* timestamp = new Timestamp();
  int64_t us = std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::system_clock::now().time_since_epoch())
                   .count();
  timestamp->set_useconds(us);
  timestamp->set_seconds(us / 1000000);

  warble->set_id("warble_id_" + std::to_string(us));
  warble->set_allocated_timestamp(timestamp);

  std::vector<std::string> warble_tags = GetHashtags(*warble);
  for (const auto& tag : warble_tags) {
    LOG(INFO) << "Found Tag: " << tag << std::endl;
    // only want latest tag, remove outdated ones
    kvstore_.Remove(tag + "_tag");
    kvstore_.Put(tag + "_tag", warble->SerializeAsString());
  }

  kvstore_.Put(warble->id() + "_content", warble->SerializeAsString());
  std::string son_ids = kvstore_.Get(std::vector<std::string>(1, parent_id))[0];
  kvstore_.Put(parent_id, son_ids + " " + warble->id());
  WarbleReply reply;
  reply.set_allocated_warble(warble);

  return reply.SerializeAsString();
}

// store to_follow into username's following, store username to to_follow's
// follower
std::optional<std::string> WarbleServer::Follow(
    const google::protobuf::Any& payload) {
  FollowRequest request;
  payload.UnpackTo(&request);
  std::string username = request.username();

  if (!ValidateUser(username)) {
    LOG(ERROR) << "the user " << username << "is not registered" << std::endl;
    return "the user is not registered";
  }

  std::string to_follow = request.to_follow();

  if (username == to_follow) {
    return "cannot follow yourself";
  }
  std::string followings =
      kvstore_.Get(std::vector<std::string>(1, username + "_following"))[0];
  std::stringstream ss(followings);
  if (!Check(ss, to_follow)) {
    if (!ValidateUser(to_follow)) {
      return "cannot follow because your target is not registered";
    }
    kvstore_.Put(username + "_following", followings + " " + to_follow);
    std::string followers =
        kvstore_.Get(std::vector<std::string>(1, to_follow + "_follower"))[0];
    kvstore_.Put(to_follow + "_follower", followers + " " + username);
    LOG(INFO) << "successfully follow " << to_follow << std::endl;
  } else {
    LOG(WARNING) << "follow " << to_follow << " again" << std::endl;
    return "follow " + to_follow + " again";
  }

  return std::nullopt;
}

// return the warble thread given id
std::optional<std::string> WarbleServer::Read(
    const google::protobuf::Any& payload) {
  ReadRequest request;
  payload.UnpackTo(&request);

  std::string username = request.username();

  if (!ValidateUser(username)) {
    LOG(ERROR) << "the user " << username << "is not registered" << std::endl;
    return "the user is not registered";
  }

  std::string root_id = request.warble_id();

  std::string content =
      kvstore_.Get(std::vector<std::string>(1, root_id + "_content"))[0];
  if (content.empty()) {
    return "read non-existing id";
  }

  ReadReply reply;
  std::queue<std::string> q;
  q.push(root_id);
  while (!q.empty()) {
    std::string curr_id = q.front();
    q.pop();
    Warble obj;
    obj.ParseFromString(
        kvstore_.Get(std::vector<std::string>(1, curr_id + "_content"))[0]);
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
std::optional<std::string> WarbleServer::Profile(
    const google::protobuf::Any& payload) {
  ProfileRequest request;
  payload.UnpackTo(&request);
  std::string username = request.username();

  if (!ValidateUser(username)) {
    LOG(ERROR) << "the user " << username << "is not registered" << std::endl;
    return "the user is not registered";
  }

  ProfileReply reply;
  std::string followings =
      kvstore_.Get(std::vector<std::string>(1, username + "_following"))[0];
  std::string followers =
      kvstore_.Get(std::vector<std::string>(1, username + "_follower"))[0];
  std::stringstream ss_followings(followings);
  std::stringstream ss_followers(followers);
  std::string tmp;
  while (getline(ss_followings, tmp, ' ')) {
    if (!tmp.empty()) {
      reply.add_following(tmp);
    }
  }
  while (getline(ss_followers, tmp, ' ')) {
    if (!tmp.empty()) {
      reply.add_followers(tmp);
    }
  }
  return reply.SerializeAsString();
}

// return the latest warble with given hashtag
std::optional<std::string> WarbleServer::Stream(
    const google::protobuf::Any& payload) {
  StreamRequest request;
  payload.UnpackTo(&request);

  Warble* warble = new Warble();
  bool valid_warble = (*warble).ParseFromString(
      kvstore_.Get(std::vector<std::string>(1, request.tag() + "_tag"))[0]);

  // did not find warbles with this hashtag
  if (!valid_warble) {
    // grpc does not deallocate for us if we don't set_allocated
    delete warble;
    return std::nullopt;
  }

  // found a warble but it isn't the latest so don't stream again or was warbled
  // before streamrequest was made
  std::vector<std::string> time_history =
      kvstore_.Get(std::vector<std::string>(1, request.tag() + "_timestamp"));

  if (time_history.size() != 0) {
    Timestamp latest_streamed_timestamp;
    latest_streamed_timestamp.ParseFromString(time_history[0]);
    if ((warble->timestamp().useconds() <=
         latest_streamed_timestamp.useconds()) ||
        (request.timestamp().useconds() > warble->timestamp().useconds())) {
      delete warble;
      return std::nullopt;
    }
  }

  StreamReply reply;
  (&reply)->set_allocated_warble(warble);
  // keep track that this is the latest warble we stream
  kvstore_.Put(request.tag() + "_timestamp",
               (warble->timestamp()).SerializeAsString());

  return reply.SerializeAsString();
}

bool WarbleServer::Check(std::stringstream& ss, const std::string& toCheck) {
  bool exist = false;
  std::string tmp;
  while (getline(ss, tmp, ' ')) {
    if (tmp == toCheck) {
      exist = true;
      break;
    }
  }
  return exist;
}

bool WarbleServer::ValidateUser(const std::string& username) {
  std::string usernames = kvstore_.Get(std::vector<std::string>(1, "users"))[0];
  std::stringstream ss(usernames);
  return Check(ss, username);
}

std::vector<std::string> WarbleServer::GetHashtags(const Warble& warble) {
  std::string warble_text = warble.text();
  regex pattern("#\\w+");
  std::sregex_iterator next(warble_text.begin(), warble_text.end(), pattern);
  std::sregex_iterator end;
  std::vector<std::string> tags;
  while (next != end) {
    std::smatch match = *next;
    tags.push_back(match.str());
    next++;
  }
  return tags;
}
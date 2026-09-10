// goal_manager.cpp - Deterministic, thread-safe goal management for ElizaOS
#include "elizaos/goal_manager.hpp"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <iomanip>
#include <limits>
#include <queue>
#include <sstream>
#include <stdexcept>

namespace elizaos {
namespace {
std::atomic<unsigned long long> nextGoalId{1};
constexpr const char* kFormatHeader = "ELIZAOS_GOALS_V3";
constexpr const char* kLegacyFormatHeader = "ELIZAOS_GOALS_V2";
constexpr std::size_t kMaxSerializedGoals = 100000;

bool validPriority(GoalPriority p) noexcept { const int v=static_cast<int>(p); return v>=0&&v<=4; }
bool validStatus(GoalStatus s) noexcept { const int v=static_cast<int>(s); return v>=0&&v<=6; }
bool validType(GoalType t) noexcept { const int v=static_cast<int>(t); return v>=0&&v<=4; }
bool terminal(GoalStatus s) noexcept { return s==GoalStatus::COMPLETED||s==GoalStatus::FAILED||s==GoalStatus::CANCELLED; }
bool legalTransition(GoalStatus from, GoalStatus to) noexcept {
    if (from==to) return true;
    switch(from) {
        case GoalStatus::PENDING: return to==GoalStatus::ACTIVE||to==GoalStatus::IN_PROGRESS||to==GoalStatus::BLOCKED||to==GoalStatus::COMPLETED||to==GoalStatus::FAILED||to==GoalStatus::CANCELLED;
        case GoalStatus::ACTIVE: return to==GoalStatus::IN_PROGRESS||to==GoalStatus::BLOCKED||to==GoalStatus::COMPLETED||to==GoalStatus::FAILED||to==GoalStatus::CANCELLED;
        case GoalStatus::IN_PROGRESS: return to==GoalStatus::ACTIVE||to==GoalStatus::BLOCKED||to==GoalStatus::COMPLETED||to==GoalStatus::FAILED||to==GoalStatus::CANCELLED;
        case GoalStatus::BLOCKED: return to==GoalStatus::PENDING||to==GoalStatus::FAILED||to==GoalStatus::CANCELLED;
        case GoalStatus::COMPLETED: case GoalStatus::FAILED: case GoalStatus::CANCELLED: return false;
    }
    return false;
}
void boundedNonempty(const std::string& v,std::size_t max,const char* field) { if(v.empty()||v.size()>max) throw std::invalid_argument(std::string(field)+" must be nonempty and bounded"); }
void bounded(const std::string& v,std::size_t max,const char* field) { if(v.size()>max) throw std::invalid_argument(std::string(field)+" is too long"); }
void finite(double v,const char* field) { if(!std::isfinite(v)) throw std::invalid_argument(std::string(field)+" must be finite"); }
long long nanos(Timestamp v) { return std::chrono::duration_cast<std::chrono::nanoseconds>(v.time_since_epoch()).count(); }
Timestamp timestampFromNanos(long long v) { return Timestamp(std::chrono::duration_cast<Timestamp::duration>(std::chrono::nanoseconds(v))); }

bool parseSize(const std::string& text,std::size_t& out) { try { std::size_t n=0; auto v=std::stoull(text,&n); if(n!=text.size()||v>kMaxSerializedGoals)return false; out=static_cast<std::size_t>(v); return true; } catch(...) { return false; } }
bool parseDouble(const std::string& text,double& out) { try { std::size_t n=0; out=std::stod(text,&n); return n==text.size()&&std::isfinite(out); } catch(...) { return false; } }
bool parseLong(const std::string& text,long long& out) { try { std::size_t n=0; out=std::stoll(text,&n); return n==text.size(); } catch(...) { return false; } }
template<class E> bool parseEnum(const std::string& text,int min,int max,E& out) { long long v=0; if(!parseLong(text,v)||v<min||v>max)return false; out=static_cast<E>(v); return true; }
bool nextLine(std::istringstream& in,const std::string& prefix,std::string& payload) { std::string line; if(!std::getline(in,line)||line.rfind(prefix,0)!=0)return false; payload=line.substr(prefix.size()); return true; }
bool quotedValue(const std::string& payload,std::string& out) { std::istringstream in(payload); if(!(in>>std::quoted(out)))return false; in>>std::ws; return in.eof(); }
std::string hexEncode(const std::string& value) {
    if (value.empty()) return "-";
    static constexpr char digits[] = "0123456789abcdef";
    std::string encoded;
    encoded.reserve(value.size() * 2);
    for (const unsigned char byte : value) {
        encoded.push_back(digits[byte >> 4]);
        encoded.push_back(digits[byte & 0x0f]);
    }
    return encoded;
}
int hexNibble(char value) noexcept {
    if (value >= '0' && value <= '9') return value - '0';
    if (value >= 'a' && value <= 'f') return value - 'a' + 10;
    if (value >= 'A' && value <= 'F') return value - 'A' + 10;
    return -1;
}
bool hexValue(const std::string& payload, std::string& out) {
    if (payload == "-") {
        out.clear();
        return true;
    }
    if (payload.size() % 2 != 0) return false;
    std::string decoded;
    decoded.reserve(payload.size() / 2);
    for (std::size_t index = 0; index < payload.size(); index += 2) {
        const int high = hexNibble(payload[index]);
        const int low = hexNibble(payload[index + 1]);
        if (high < 0 || low < 0) return false;
        decoded.push_back(static_cast<char>((high << 4) | low));
    }
    out = std::move(decoded);
    return true;
}
void observeRestoredId(const UUID& id) {
    constexpr const char* prefix = "goal_";
    if (id.rfind(prefix, 0) != 0) return;
    try {
        std::size_t consumed = 0;
        const auto value = std::stoull(id.substr(5), &consumed);
        if (consumed != id.size() - 5 || value == std::numeric_limits<unsigned long long>::max()) return;
        auto expected = nextGoalId.load(std::memory_order_relaxed);
        const auto desired = value + 1;
        while (expected < desired && !nextGoalId.compare_exchange_weak(
            expected, desired, std::memory_order_relaxed, std::memory_order_relaxed)) {}
    } catch (...) {}
}
}

std::string goalStatusToString(GoalStatus s) { switch(s){case GoalStatus::PENDING:return"PENDING";case GoalStatus::ACTIVE:return"ACTIVE";case GoalStatus::IN_PROGRESS:return"IN_PROGRESS";case GoalStatus::BLOCKED:return"BLOCKED";case GoalStatus::COMPLETED:return"COMPLETED";case GoalStatus::FAILED:return"FAILED";case GoalStatus::CANCELLED:return"CANCELLED";} return"UNKNOWN"; }
GoalStatus stringToGoalStatus(const std::string& s) { if(s=="ACTIVE")return GoalStatus::ACTIVE;if(s=="IN_PROGRESS")return GoalStatus::IN_PROGRESS;if(s=="BLOCKED")return GoalStatus::BLOCKED;if(s=="COMPLETED")return GoalStatus::COMPLETED;if(s=="FAILED")return GoalStatus::FAILED;if(s=="CANCELLED")return GoalStatus::CANCELLED;return GoalStatus::PENDING; }
std::string goalPriorityToString(GoalPriority p) { switch(p){case GoalPriority::CRITICAL:return"CRITICAL";case GoalPriority::HIGH:return"HIGH";case GoalPriority::NORMAL:return"NORMAL";case GoalPriority::LOW:return"LOW";case GoalPriority::BACKGROUND:return"BACKGROUND";} return"NORMAL"; }
GoalPriority stringToGoalPriority(const std::string& s) { if(s=="CRITICAL")return GoalPriority::CRITICAL;if(s=="HIGH")return GoalPriority::HIGH;if(s=="LOW")return GoalPriority::LOW;if(s=="BACKGROUND")return GoalPriority::BACKGROUND;return GoalPriority::NORMAL; }
std::string goalTypeToString(GoalType t) { switch(t){case GoalType::ACHIEVEMENT:return"ACHIEVEMENT";case GoalType::MAINTENANCE:return"MAINTENANCE";case GoalType::IMPROVEMENT:return"IMPROVEMENT";case GoalType::EXPLORATION:return"EXPLORATION";case GoalType::REACTIVE:return"REACTIVE";} return"ACHIEVEMENT"; }
GoalType stringToGoalType(const std::string& s) { if(s=="MAINTENANCE")return GoalType::MAINTENANCE;if(s=="IMPROVEMENT")return GoalType::IMPROVEMENT;if(s=="EXPLORATION")return GoalType::EXPLORATION;if(s=="REACTIVE")return GoalType::REACTIVE;return GoalType::ACHIEVEMENT; }

Goal::Goal(const std::string& n,const std::string& d):Goal(generateId(),n,d){}
Goal::Goal(const UUID& id,const std::string& n,const std::string& d):id_(id),name_(n),description_(d),createdAt_(std::chrono::system_clock::now()),updatedAt_(createdAt_) { boundedNonempty(id_,MAX_ID_LENGTH,"goal id"); boundedNonempty(name_,MAX_NAME_LENGTH,"goal name"); bounded(description_,MAX_DESCRIPTION_LENGTH,"goal description"); }
UUID Goal::generateId(){const auto v=nextGoalId.fetch_add(1,std::memory_order_relaxed);if(v==std::numeric_limits<unsigned long long>::max())throw std::overflow_error("goal ID space exhausted");std::ostringstream out;out<<"goal_"<<std::setw(20)<<std::setfill('0')<<v;return out.str();}
void Goal::updateTimestampUnlocked(){updatedAt_=std::chrono::system_clock::now();}
bool Goal::transitionToUnlocked(GoalStatus s){if(!validStatus(s)||!legalTransition(status_,s))return false;if(status_==s)return true;status_=s;if(s==GoalStatus::COMPLETED){progress_=1.0;completedAt_=std::chrono::system_clock::now();}else completedAt_.reset();updateTimestampUnlocked();return true;}
GoalStatus Goal::getStatus()const{std::lock_guard<std::mutex>l(stateMutex_);return status_;}
void Goal::setStatus(GoalStatus s){std::lock_guard<std::mutex>l(stateMutex_);if(!transitionToUnlocked(s))throw std::logic_error("illegal goal status transition");}
bool Goal::isActive()const{auto s=getStatus();return s==GoalStatus::ACTIVE||s==GoalStatus::IN_PROGRESS;}
bool Goal::isCompleted()const{return getStatus()==GoalStatus::COMPLETED;}
bool Goal::isBlocked()const{return getStatus()==GoalStatus::BLOCKED;}
GoalPriority Goal::getPriority()const{std::lock_guard<std::mutex>l(stateMutex_);return priority_;}
void Goal::setPriority(GoalPriority p){if(!validPriority(p))throw std::invalid_argument("invalid goal priority");std::lock_guard<std::mutex>l(stateMutex_);priority_=p;updateTimestampUnlocked();}
int Goal::getPriorityValue()const{return static_cast<int>(getPriority());}
GoalType Goal::getType()const{std::lock_guard<std::mutex>l(stateMutex_);return type_;}
void Goal::setType(GoalType t){if(!validType(t))throw std::invalid_argument("invalid goal type");std::lock_guard<std::mutex>l(stateMutex_);type_=t;updateTimestampUnlocked();}
double Goal::getProgress()const{std::lock_guard<std::mutex>l(stateMutex_);return progress_;}
void Goal::setProgress(double p){finite(p,"progress");std::lock_guard<std::mutex>l(stateMutex_);if(terminal(status_))throw std::logic_error("terminal goal progress is immutable");progress_=std::clamp(p,0.0,1.0);if(progress_>0.0&&status_==GoalStatus::ACTIVE)transitionToUnlocked(GoalStatus::IN_PROGRESS);else updateTimestampUnlocked();}
void Goal::incrementProgress(double a){finite(a,"progress increment");std::lock_guard<std::mutex>l(stateMutex_);if(terminal(status_))throw std::logic_error("terminal goal progress is immutable");progress_=std::clamp(progress_+a,0.0,1.0);if(progress_>0.0&&status_==GoalStatus::ACTIVE)transitionToUnlocked(GoalStatus::IN_PROGRESS);else updateTimestampUnlocked();}
void Goal::addDependency(const UUID& id){boundedNonempty(id,MAX_ID_LENGTH,"dependency id");if(id==id_)throw std::invalid_argument("self dependency");std::lock_guard<std::mutex>l(stateMutex_);if(dependencies_.size()>=MAX_DEPENDENCY_COUNT&&!dependencies_.count(id))throw std::length_error("dependency limit exceeded");dependencies_.insert(id);updateTimestampUnlocked();}
void Goal::removeDependency(const UUID& id){std::lock_guard<std::mutex>l(stateMutex_);if(dependencies_.erase(id))updateTimestampUnlocked();}
std::unordered_set<UUID> Goal::getDependenciesCopy()const{std::lock_guard<std::mutex>l(stateMutex_);return dependencies_;}
bool Goal::hasDependencies()const{std::lock_guard<std::mutex>l(stateMutex_);return!dependencies_.empty();}
void Goal::addSubGoal(std::shared_ptr<Goal> child){if(!child||child->getId()==id_)throw std::invalid_argument("invalid sub-goal");std::queue<std::shared_ptr<Goal>>todo;std::unordered_set<UUID>seen;todo.push(child);while(!todo.empty()){auto current=todo.front();todo.pop();if(current->getId()==id_)throw std::logic_error("cyclic sub-goal hierarchy");if(!seen.insert(current->getId()).second)continue;for(const auto&descendant:current->getSubGoalsCopy())todo.push(descendant);}auto self=shared_from_this();std::scoped_lock l(stateMutex_,child->stateMutex_);auto it=std::find_if(subGoals_.begin(),subGoals_.end(),[&](const auto&g){return g->getId()==child->getId();});if(it!=subGoals_.end())return;if(auto parent=child->parentGoal_.lock();parent&&parent->getId()!=id_)throw std::logic_error("sub-goal already parented");child->parentGoal_=self;child->updateTimestampUnlocked();subGoals_.push_back(std::move(child));updateTimestampUnlocked();}
void Goal::removeSubGoal(const UUID& id){std::shared_ptr<Goal> child;{std::lock_guard<std::mutex>l(stateMutex_);auto it=std::find_if(subGoals_.begin(),subGoals_.end(),[&](const auto&g){return g->getId()==id;});if(it==subGoals_.end())return;child=*it;subGoals_.erase(it);updateTimestampUnlocked();}{std::lock_guard<std::mutex>l(child->stateMutex_);child->parentGoal_.reset();child->updateTimestampUnlocked();}}
std::vector<std::shared_ptr<Goal>> Goal::getSubGoalsCopy()const{std::lock_guard<std::mutex>l(stateMutex_);return subGoals_;}
std::shared_ptr<Goal> Goal::getParentGoal()const{std::lock_guard<std::mutex>l(stateMutex_);return parentGoal_.lock();}
void Goal::setParentGoal(std::shared_ptr<Goal>p){if(p&&p->getId()==id_)throw std::invalid_argument("self parent");std::unordered_set<UUID>seen;for(auto current=p;current;current=current->getParentGoal()){if(current->getId()==id_)throw std::logic_error("cyclic parent hierarchy");if(!seen.insert(current->getId()).second)throw std::logic_error("cyclic parent chain");}std::lock_guard<std::mutex>l(stateMutex_);parentGoal_=std::move(p);updateTimestampUnlocked();}
bool Goal::hasSubGoals()const{std::lock_guard<std::mutex>l(stateMutex_);return!subGoals_.empty();}
void Goal::setDeadline(const Timestamp&d){std::lock_guard<std::mutex>l(stateMutex_);if(d<createdAt_)throw std::invalid_argument("deadline precedes creation");deadline_=d;updateTimestampUnlocked();}
std::optional<Timestamp> Goal::getDeadline()const{std::lock_guard<std::mutex>l(stateMutex_);return deadline_;}
bool Goal::hasDeadline()const{return getDeadline().has_value();}
bool Goal::isOverdue()const{auto d=getDeadline();return d&&std::chrono::system_clock::now()>*d;}
std::chrono::duration<double> Goal::getTimeRemaining()const{auto d=getDeadline();return d?std::chrono::duration<double>(*d-std::chrono::system_clock::now()):std::chrono::duration<double>::max();}
Timestamp Goal::getCreatedAt()const{std::lock_guard<std::mutex>l(stateMutex_);return createdAt_;}
Timestamp Goal::getUpdatedAt()const{std::lock_guard<std::mutex>l(stateMutex_);return updatedAt_;}
std::optional<Timestamp> Goal::getCompletedAt()const{std::lock_guard<std::mutex>l(stateMutex_);return completedAt_;}
void Goal::addTag(const std::string&t){boundedNonempty(t,MAX_TAG_LENGTH,"tag");std::lock_guard<std::mutex>l(stateMutex_);if(tags_.size()>=MAX_TAG_COUNT&&!tags_.count(t))throw std::length_error("tag limit exceeded");tags_.insert(t);updateTimestampUnlocked();}
void Goal::removeTag(const std::string&t){std::lock_guard<std::mutex>l(stateMutex_);if(tags_.erase(t))updateTimestampUnlocked();}
std::unordered_set<std::string> Goal::getTagsCopy()const{std::lock_guard<std::mutex>l(stateMutex_);return tags_;}
bool Goal::hasTag(const std::string&t)const{std::lock_guard<std::mutex>l(stateMutex_);return tags_.count(t)!=0;}
void Goal::setMetadata(const std::string&k,const std::string&v){boundedNonempty(k,MAX_METADATA_KEY_LENGTH,"metadata key");bounded(v,MAX_METADATA_VALUE_LENGTH,"metadata value");std::lock_guard<std::mutex>l(stateMutex_);if(metadata_.size()>=MAX_METADATA_COUNT&&!metadata_.count(k))throw std::length_error("metadata limit exceeded");metadata_[k]=v;updateTimestampUnlocked();}
std::optional<std::string> Goal::getMetadata(const std::string&k)const{std::lock_guard<std::mutex>l(stateMutex_);auto it=metadata_.find(k);return it==metadata_.end()?std::nullopt:std::optional<std::string>(it->second);}
std::unordered_map<std::string,std::string> Goal::getAllMetadataCopy()const{std::lock_guard<std::mutex>l(stateMutex_);return metadata_;}
double Goal::getReward()const{std::lock_guard<std::mutex>l(stateMutex_);return reward_;}
void Goal::setReward(double v){finite(v,"reward");if(v<0.0)throw std::invalid_argument("negative reward");std::lock_guard<std::mutex>l(stateMutex_);reward_=v;updateTimestampUnlocked();}
double Goal::getEstimatedEffort()const{std::lock_guard<std::mutex>l(stateMutex_);return estimatedEffort_;}
void Goal::setEstimatedEffort(double v){finite(v,"estimated effort");if(v<=0.0)throw std::invalid_argument("estimated effort must be positive");std::lock_guard<std::mutex>l(stateMutex_);estimatedEffort_=v;updateTimestampUnlocked();}
double Goal::getActualEffort()const{std::lock_guard<std::mutex>l(stateMutex_);return actualEffort_;}
void Goal::addActualEffort(double v){finite(v,"actual effort");if(v<0.0)throw std::invalid_argument("negative actual effort");std::lock_guard<std::mutex>l(stateMutex_);if(!std::isfinite(actualEffort_+v))throw std::overflow_error("actual effort overflow");actualEffort_+=v;updateTimestampUnlocked();}
GoalSnapshot Goal::snapshot()const{std::lock_guard<std::mutex>l(stateMutex_);GoalSnapshot s;s.id=id_;s.name=name_;s.description=description_;s.status=status_;s.priority=priority_;s.type=type_;s.progress=progress_;s.dependencies.assign(dependencies_.begin(),dependencies_.end());for(const auto&g:subGoals_)s.subGoalIds.push_back(g->getId());if(auto p=parentGoal_.lock())s.parentGoalId=p->getId();s.deadline=deadline_;s.createdAt=createdAt_;s.updatedAt=updatedAt_;s.completedAt=completedAt_;s.tags.assign(tags_.begin(),tags_.end());s.metadata.assign(metadata_.begin(),metadata_.end());s.reward=reward_;s.estimatedEffort=estimatedEffort_;s.actualEffort=actualEffort_;std::sort(s.dependencies.begin(),s.dependencies.end());std::sort(s.subGoalIds.begin(),s.subGoalIds.end());std::sort(s.tags.begin(),s.tags.end());std::sort(s.metadata.begin(),s.metadata.end());return s;}
double Goal::calculateScore()const{auto s=snapshot();double score=(4-static_cast<int>(s.priority))*100.0;if(s.deadline){double r=std::chrono::duration<double>(*s.deadline-std::chrono::system_clock::now()).count();if(r<=0)score+=1000;else if(r<3600)score+=500;else if(r<86400)score+=250;else score+=1.0/std::max(r/86400.0,1.0);}score+=(s.reward/s.estimatedEffort)*10.0+s.progress*5.0;if(s.status==GoalStatus::ACTIVE||s.status==GoalStatus::IN_PROGRESS)score+=20.0;if(s.status==GoalStatus::BLOCKED)score-=10000.0;return score;}

GoalManager::GoalManager()=default;
GoalManager::~GoalManager()=default;

std::shared_ptr<Goal> GoalManager::createGoal(const std::string& n,const std::string& d){return createGoal(n,d,GoalPriority::NORMAL,GoalType::ACHIEVEMENT);}
std::shared_ptr<Goal> GoalManager::createGoal(const std::string& n,const std::string& d,GoalPriority p,GoalType t){auto goal=std::make_shared<Goal>(n,d);goal->setPriority(p);goal->setType(t);{std::lock_guard<std::mutex>l(mutex_);if(!goals_.emplace(goal->getId(),goal).second)throw std::logic_error("duplicate generated goal ID");}notifyCreated(goal);return goal;}
bool GoalManager::removeGoal(const UUID& id){std::lock_guard<std::mutex>l(mutex_);if(!goals_.count(id))return false;std::unordered_set<UUID>removing;std::queue<UUID>todo;todo.push(id);while(!todo.empty()){auto current=todo.front();todo.pop();if(!removing.insert(current).second)continue;auto it=goals_.find(current);if(it!=goals_.end())for(const auto&child:it->second->getSubGoalsCopy())todo.push(child->getId());}for(auto&e:goals_){if(removing.count(e.first))continue;bool lostPrerequisite=false;for(const auto&r:removing)if(e.second->getDependenciesCopy().count(r)){e.second->removeDependency(r);lostPrerequisite=true;}if(lostPrerequisite&&!terminal(e.second->getStatus()))e.second->setStatus(GoalStatus::CANCELLED);for(const auto&child:e.second->getSubGoalsCopy())if(removing.count(child->getId()))e.second->removeSubGoal(child->getId());}for(const auto&r:removing)goals_.erase(r);updateBlockedGoalsUnlocked();return true;}
std::shared_ptr<Goal> GoalManager::getGoal(const UUID&id)const{std::lock_guard<std::mutex>l(mutex_);auto it=goals_.find(id);return it==goals_.end()?nullptr:it->second;}
std::optional<GoalSnapshot> GoalManager::getGoalSnapshot(const UUID&id)const{auto g=getGoal(id);return g?std::optional<GoalSnapshot>(g->snapshot()):std::nullopt;}
bool GoalManager::hasGoal(const UUID&id)const{std::lock_guard<std::mutex>l(mutex_);return goals_.count(id)!=0;}
std::vector<std::shared_ptr<Goal>> GoalManager::getAllGoals()const{std::lock_guard<std::mutex>l(mutex_);std::vector<std::shared_ptr<Goal>>out;for(const auto&e:goals_)out.push_back(e.second);std::sort(out.begin(),out.end(),[](const auto&a,const auto&b){return a->getId()<b->getId();});return out;}
std::vector<GoalSnapshot> GoalManager::getAllGoalSnapshots()const{std::lock_guard<std::mutex>l(mutex_);std::vector<GoalSnapshot>out;out.reserve(goals_.size());for(const auto&e:goals_)out.push_back(e.second->snapshot());std::sort(out.begin(),out.end(),[](const auto&a,const auto&b){return a.id<b.id;});return out;}
std::vector<std::shared_ptr<Goal>> GoalManager::getGoalsByStatus(GoalStatus s)const{auto all=getAllGoals();all.erase(std::remove_if(all.begin(),all.end(),[s](const auto&g){return g->getStatus()!=s;}),all.end());return all;}
std::vector<std::shared_ptr<Goal>> GoalManager::getGoalsByPriority(GoalPriority p)const{auto all=getAllGoals();all.erase(std::remove_if(all.begin(),all.end(),[p](const auto&g){return g->getPriority()!=p;}),all.end());return all;}
std::vector<std::shared_ptr<Goal>> GoalManager::getGoalsByType(GoalType t)const{auto all=getAllGoals();all.erase(std::remove_if(all.begin(),all.end(),[t](const auto&g){return g->getType()!=t;}),all.end());return all;}
std::vector<std::shared_ptr<Goal>> GoalManager::getGoalsByTag(const std::string&t)const{auto all=getAllGoals();all.erase(std::remove_if(all.begin(),all.end(),[&](const auto&g){return!g->hasTag(t);}),all.end());return all;}
std::vector<std::shared_ptr<Goal>> GoalManager::getActiveGoals()const{auto all=getAllGoals();all.erase(std::remove_if(all.begin(),all.end(),[](const auto&g){return!g->isActive();}),all.end());return all;}
std::vector<std::shared_ptr<Goal>> GoalManager::getBlockedGoals()const{return getGoalsByStatus(GoalStatus::BLOCKED);}
std::vector<std::shared_ptr<Goal>> GoalManager::getOverdueGoals()const{auto all=getAllGoals();all.erase(std::remove_if(all.begin(),all.end(),[](const auto&g){return!g->isOverdue()||terminal(g->getStatus());}),all.end());return all;}
std::shared_ptr<Goal> GoalManager::selectNextGoal()const{auto v=getTopGoals(1);return v.empty()?nullptr:v.front();}
std::vector<std::shared_ptr<Goal>> GoalManager::getTopGoals(std::size_t count)const{std::lock_guard<std::mutex>l(mutex_);std::vector<std::shared_ptr<Goal>>eligible;for(const auto&e:goals_){auto s=e.second->getStatus();if((s==GoalStatus::PENDING||s==GoalStatus::ACTIVE||s==GoalStatus::IN_PROGRESS)&&dependenciesMetUnlocked(e.second))eligible.push_back(e.second);}std::sort(eligible.begin(),eligible.end(),[](const auto&a,const auto&b){double as=a->calculateScore(),bs=b->calculateScore();if(as!=bs)return as>bs;auto ad=a->getDeadline(),bd=b->getDeadline();if(ad!=bd){if(!ad)return false;if(!bd)return true;return *ad<*bd;}return a->getId()<b->getId();});if(eligible.size()>count)eligible.resize(count);return eligible;}
bool GoalManager::dependenciesMetUnlocked(const std::shared_ptr<Goal>&g)const{for(const auto&id:g->getDependenciesCopy()){auto it=goals_.find(id);if(it==goals_.end()||!it->second->isCompleted())return false;}return true;}
bool GoalManager::detectCycle(const UUID&goalId,const UUID&depId)const{std::queue<UUID>todo;std::unordered_set<UUID>seen;todo.push(depId);while(!todo.empty()){auto id=todo.front();todo.pop();if(id==goalId)return true;if(!seen.insert(id).second)continue;auto it=goals_.find(id);if(it!=goals_.end())for(const auto&dep:it->second->getDependenciesCopy())todo.push(dep);}return false;}
bool GoalManager::addDependency(const UUID&goalId,const UUID&depId){std::lock_guard<std::mutex>l(mutex_);if(goalId==depId)return false;auto g=goals_.find(goalId),d=goals_.find(depId);if(g==goals_.end()||d==goals_.end()||terminal(g->second->getStatus())||detectCycle(goalId,depId)||g->second->getDependenciesCopy().count(depId))return false;g->second->addDependency(depId);auto s=g->second->getStatus();if((s==GoalStatus::ACTIVE||s==GoalStatus::IN_PROGRESS)&&!dependenciesMetUnlocked(g->second))g->second->setStatus(GoalStatus::BLOCKED);return true;}
bool GoalManager::removeDependency(const UUID&goalId,const UUID&depId){std::lock_guard<std::mutex>l(mutex_);auto it=goals_.find(goalId);if(it==goals_.end()||!it->second->getDependenciesCopy().count(depId))return false;it->second->removeDependency(depId);if(it->second->getStatus()==GoalStatus::BLOCKED&&dependenciesMetUnlocked(it->second))it->second->setStatus(GoalStatus::PENDING);return true;}
bool GoalManager::areDependenciesMet(const UUID&id)const{std::lock_guard<std::mutex>l(mutex_);auto it=goals_.find(id);return it!=goals_.end()&&dependenciesMetUnlocked(it->second);}
std::vector<UUID> GoalManager::getUnmetDependencies(const UUID&id)const{std::lock_guard<std::mutex>l(mutex_);std::vector<UUID>out;auto it=goals_.find(id);if(it==goals_.end())return out;for(const auto&dep:it->second->getDependenciesCopy()){auto found=goals_.find(dep);if(found==goals_.end()||!found->second->isCompleted())out.push_back(dep);}std::sort(out.begin(),out.end());return out;}
void GoalManager::updateBlockedGoalsUnlocked(){for(auto&e:goals_)if(e.second->getStatus()==GoalStatus::BLOCKED&&dependenciesMetUnlocked(e.second))e.second->setStatus(GoalStatus::PENDING);}
void GoalManager::updateBlockedGoals(){std::lock_guard<std::mutex>l(mutex_);updateBlockedGoalsUnlocked();}
bool GoalManager::hierarchyContainsUnlocked(const UUID&root,const UUID&sought)const{std::queue<UUID>todo;std::unordered_set<UUID>seen;todo.push(root);while(!todo.empty()){auto id=todo.front();todo.pop();if(id==sought)return true;if(!seen.insert(id).second)continue;auto it=goals_.find(id);if(it!=goals_.end())for(const auto&child:it->second->getSubGoalsCopy())todo.push(child->getId());}return false;}
bool GoalManager::addSubGoal(const UUID&parentId,const UUID&childId){std::lock_guard<std::mutex>l(mutex_);if(parentId==childId)return false;auto p=goals_.find(parentId),c=goals_.find(childId);if(p==goals_.end()||c==goals_.end()||terminal(p->second->getStatus())||hierarchyContainsUnlocked(childId,parentId))return false;if(auto existing=c->second->getParentGoal();existing&&existing->getId()!=parentId)return false;for(const auto&g:p->second->getSubGoalsCopy())if(g->getId()==childId)return false;p->second->addSubGoal(c->second);return true;}
bool GoalManager::removeSubGoal(const UUID&parentId,const UUID&childId){std::lock_guard<std::mutex>l(mutex_);auto p=goals_.find(parentId);if(p==goals_.end())return false;bool found=false;for(const auto&g:p->second->getSubGoalsCopy())if(g->getId()==childId)found=true;if(!found)return false;p->second->removeSubGoal(childId);return true;}
std::vector<std::shared_ptr<Goal>> GoalManager::updateAncestorProgressUnlocked(const UUID&id){std::vector<std::shared_ptr<Goal>>changed;auto it=goals_.find(id);if(it==goals_.end())return changed;auto parent=it->second->getParentGoal();std::unordered_set<UUID>seen;while(parent&&seen.insert(parent->getId()).second){auto children=parent->getSubGoalsCopy();if(children.empty())break;double total=0;for(const auto&c:children)total+=c->getProgress();if(!terminal(parent->getStatus())){parent->setProgress(total/static_cast<double>(children.size()));changed.push_back(parent);}parent=parent->getParentGoal();}return changed;}
void GoalManager::updateParentProgress(const UUID&id){std::vector<std::shared_ptr<Goal>>changed;{std::lock_guard<std::mutex>l(mutex_);changed=updateAncestorProgressUnlocked(id);}for(const auto&g:changed)notifyProgress(g);}
bool GoalManager::subGoalsCompletedUnlocked(const std::shared_ptr<Goal>&g)const{for(const auto&child:g->getSubGoalsCopy())if(!child->isCompleted())return false;return true;}
bool GoalManager::activateGoal(const UUID&id){std::lock_guard<std::mutex>l(mutex_);auto it=goals_.find(id);if(it==goals_.end()||it->second->getStatus()!=GoalStatus::PENDING)return false;if(!dependenciesMetUnlocked(it->second)){it->second->setStatus(GoalStatus::BLOCKED);return false;}it->second->setStatus(GoalStatus::ACTIVE);return true;}
bool GoalManager::completeGoal(const UUID&id){std::shared_ptr<Goal>goal;std::vector<std::shared_ptr<Goal>>parents;{std::lock_guard<std::mutex>l(mutex_);auto it=goals_.find(id);if(it==goals_.end()||terminal(it->second->getStatus())||!dependenciesMetUnlocked(it->second)||!subGoalsCompletedUnlocked(it->second))return false;goal=it->second;goal->setStatus(GoalStatus::COMPLETED);updateBlockedGoalsUnlocked();parents=updateAncestorProgressUnlocked(id);}notifyCompleted(goal);for(const auto&p:parents)notifyProgress(p);return true;}
bool GoalManager::failGoal(const UUID&id,const std::string&reason){bounded(reason,Goal::MAX_METADATA_VALUE_LENGTH,"failure reason");std::shared_ptr<Goal>goal;{std::lock_guard<std::mutex>l(mutex_);auto it=goals_.find(id);if(it==goals_.end()||terminal(it->second->getStatus()))return false;goal=it->second;goal->setStatus(GoalStatus::FAILED);if(!reason.empty())goal->setMetadata("failure_reason",reason);}notifyFailed(goal);return true;}
bool GoalManager::cancelGoal(const UUID&id){std::lock_guard<std::mutex>l(mutex_);auto root=goals_.find(id);if(root==goals_.end()||terminal(root->second->getStatus()))return false;std::queue<std::shared_ptr<Goal>>todo;std::unordered_set<UUID>seen;todo.push(root->second);while(!todo.empty()){auto g=todo.front();todo.pop();if(!seen.insert(g->getId()).second)continue;for(const auto&child:g->getSubGoalsCopy())todo.push(child);if(!terminal(g->getStatus()))g->setStatus(GoalStatus::CANCELLED);}return true;}
bool GoalManager::blockGoal(const UUID&id){std::lock_guard<std::mutex>l(mutex_);auto it=goals_.find(id);if(it==goals_.end())return false;auto s=it->second->getStatus();if(s!=GoalStatus::PENDING&&s!=GoalStatus::ACTIVE&&s!=GoalStatus::IN_PROGRESS)return false;it->second->setStatus(GoalStatus::BLOCKED);return true;}
bool GoalManager::updateProgress(const UUID&id,double progress){if(!std::isfinite(progress))return false;std::shared_ptr<Goal>goal;std::vector<std::shared_ptr<Goal>>parents;{std::lock_guard<std::mutex>l(mutex_);auto it=goals_.find(id);if(it==goals_.end()||terminal(it->second->getStatus())||it->second->getStatus()==GoalStatus::BLOCKED||!dependenciesMetUnlocked(it->second))return false;goal=it->second;goal->setProgress(progress);parents=updateAncestorProgressUnlocked(id);}notifyProgress(goal);for(const auto&p:parents)notifyProgress(p);return true;}
bool GoalManager::incrementProgress(const UUID&id,double amount){if(!std::isfinite(amount))return false;std::shared_ptr<Goal>goal;std::vector<std::shared_ptr<Goal>>parents;{std::lock_guard<std::mutex>l(mutex_);auto it=goals_.find(id);if(it==goals_.end()||terminal(it->second->getStatus())||it->second->getStatus()==GoalStatus::BLOCKED||!dependenciesMetUnlocked(it->second))return false;goal=it->second;goal->incrementProgress(amount);parents=updateAncestorProgressUnlocked(id);}notifyProgress(goal);for(const auto&p:parents)notifyProgress(p);return true;}
void GoalManager::onGoalCreated(GoalCallback cb){if(!cb)return;std::lock_guard<std::mutex>l(mutex_);onCreatedCallbacks_.push_back(std::move(cb));}
void GoalManager::onGoalCompleted(GoalCallback cb){if(!cb)return;std::lock_guard<std::mutex>l(mutex_);onCompletedCallbacks_.push_back(std::move(cb));}
void GoalManager::onGoalFailed(GoalCallback cb){if(!cb)return;std::lock_guard<std::mutex>l(mutex_);onFailedCallbacks_.push_back(std::move(cb));}
void GoalManager::onGoalProgress(GoalCallback cb){if(!cb)return;std::lock_guard<std::mutex>l(mutex_);onProgressCallbacks_.push_back(std::move(cb));}
void GoalManager::invokeCallbacks(const std::vector<GoalCallback>&callbacks,const std::shared_ptr<Goal>&g)noexcept{for(const auto&cb:callbacks)try{cb(g);}catch(...) {}}
void GoalManager::notifyCreated(const std::shared_ptr<Goal>&g){std::vector<GoalCallback>c;{std::lock_guard<std::mutex>l(mutex_);c=onCreatedCallbacks_;}invokeCallbacks(c,g);}
void GoalManager::notifyCompleted(const std::shared_ptr<Goal>&g){std::vector<GoalCallback>c;{std::lock_guard<std::mutex>l(mutex_);c=onCompletedCallbacks_;}invokeCallbacks(c,g);}
void GoalManager::notifyFailed(const std::shared_ptr<Goal>&g){std::vector<GoalCallback>c;{std::lock_guard<std::mutex>l(mutex_);c=onFailedCallbacks_;}invokeCallbacks(c,g);}
void GoalManager::notifyProgress(const std::shared_ptr<Goal>&g){std::vector<GoalCallback>c;{std::lock_guard<std::mutex>l(mutex_);c=onProgressCallbacks_;}invokeCallbacks(c,g);}
std::size_t GoalManager::getTotalGoalCount()const{std::lock_guard<std::mutex>l(mutex_);return goals_.size();}
std::size_t GoalManager::getActiveGoalCount()const{return getActiveGoals().size();}
std::size_t GoalManager::getCompletedGoalCount()const{return getGoalsByStatus(GoalStatus::COMPLETED).size();}
double GoalManager::getOverallProgress()const{auto v=getAllGoalSnapshots();if(v.empty())return 0;double total=0;for(const auto&s:v)total+=s.progress;return total/static_cast<double>(v.size());}
std::unordered_map<GoalStatus,std::size_t> GoalManager::getGoalCountByStatus()const{std::unordered_map<GoalStatus,std::size_t>out;for(const auto&s:getAllGoalSnapshots())++out[s.status];return out;}

std::string GoalManager::serialize() const {
    const auto snapshots = getAllGoalSnapshots();
    std::ostringstream out;
    out << kFormatHeader << '\n' << "COUNT " << snapshots.size() << '\n';
    out << std::setprecision(17);
    for (const auto& state : snapshots) {
        out << "BEGIN\n";
        out << "ID " << hexEncode(state.id) << '\n';
        out << "NAME " << hexEncode(state.name) << '\n';
        out << "DESCRIPTION " << hexEncode(state.description) << '\n';
        out << "STATE " << static_cast<int>(state.status) << ' '
            << static_cast<int>(state.priority) << ' '
            << static_cast<int>(state.type) << ' ' << state.progress << '\n';
        out << "TIMES " << nanos(state.createdAt) << ' ' << nanos(state.updatedAt) << ' '
            << (state.completedAt ? 1 : 0) << ' '
            << (state.completedAt ? nanos(*state.completedAt) : 0) << ' '
            << (state.deadline ? 1 : 0) << ' '
            << (state.deadline ? nanos(*state.deadline) : 0) << '\n';
        out << "EFFORT " << state.reward << ' ' << state.estimatedEffort << ' '
            << state.actualEffort << '\n';
        out << "DEPS " << state.dependencies.size() << '\n';
        for (const auto& dependency : state.dependencies) {
            out << "DEP " << hexEncode(dependency) << '\n';
        }
        out << "SUBS " << state.subGoalIds.size() << '\n';
        for (const auto& subGoal : state.subGoalIds) {
            out << "SUB " << hexEncode(subGoal) << '\n';
        }
        out << "TAGS " << state.tags.size() << '\n';
        for (const auto& tag : state.tags) {
            out << "TAG " << hexEncode(tag) << '\n';
        }
        out << "METADATA " << state.metadata.size() << '\n';
        for (const auto& item : state.metadata) {
            out << "META " << hexEncode(item.first) << ' ' << hexEncode(item.second) << '\n';
        }
        out << "END\n";
    }
    return out.str();
}

bool GoalManager::deserialize(const std::string& data) {
    try {
        std::istringstream input(data);
        std::string header;
        if (!std::getline(input, header)) {
            return false;
        }

        // Preserve compatibility with the original six-field format. It cannot
        // restore fields the old format never stored, but remains transactional.
        if (header.rfind("GOALS:", 0) == 0) {
            std::size_t expected = 0;
            if (!parseSize(header.substr(6), expected)) {
                return false;
            }
            GoalMap parsed;
            std::string line;
            while (std::getline(input, line)) {
                if (line.empty()) continue;
                std::vector<std::string> fields;
                std::istringstream row(line);
                std::string field;
                while (std::getline(row, field, '|')) fields.push_back(field);
                if (fields.size() != 6 || fields[0] != "GOAL") return false;
                boundedNonempty(fields[1], Goal::MAX_ID_LENGTH, "goal id");
                boundedNonempty(fields[2], Goal::MAX_NAME_LENGTH, "goal name");
                GoalStatus status{};
                GoalPriority priority{};
                double progress = 0.0;
                if (!parseEnum(fields[3], 0, 6, status)) {
                    const auto parsedStatus = stringToGoalStatus(fields[3]);
                    if (goalStatusToString(parsedStatus) != fields[3]) return false;
                    status = parsedStatus;
                }
                if (!parseEnum(fields[4], 0, 4, priority)) {
                    const auto parsedPriority = stringToGoalPriority(fields[4]);
                    if (goalPriorityToString(parsedPriority) != fields[4]) return false;
                    priority = parsedPriority;
                }
                if (!parseDouble(fields[5], progress) || progress < 0.0 || progress > 1.0 ||
                    (status == GoalStatus::COMPLETED && progress != 1.0)) return false;
                auto goal = std::make_shared<Goal>(fields[1], fields[2], "");
                {
                    std::lock_guard<std::mutex> goalLock(goal->stateMutex_);
                    goal->status_ = status;
                    goal->priority_ = priority;
                    goal->progress_ = progress;
                    if (status == GoalStatus::COMPLETED) goal->completedAt_ = goal->updatedAt_;
                }
                if (!parsed.emplace(goal->getId(), goal).second) return false;
            }
            if (parsed.size() != expected) return false;
            std::lock_guard<std::mutex> lock(mutex_);
            goals_.swap(parsed);
            for (const auto& entry : goals_) observeRestoredId(entry.first);
            return true;
        }

        const bool encodedStrings = header == kFormatHeader;
        if (!encodedStrings && header != kLegacyFormatHeader) return false;
        const auto parseString = [&](const std::string& value, std::string& output) {
            return encodedStrings ? hexValue(value, output) : quotedValue(value, output);
        };
        std::string payload;
        if (!nextLine(input, "COUNT ", payload)) return false;
        std::size_t expected = 0;
        if (!parseSize(payload, expected)) return false;

        std::vector<GoalSnapshot> states;
        states.reserve(expected);
        for (std::size_t index = 0; index < expected; ++index) {
            std::string line;
            if (!std::getline(input, line) || line != "BEGIN") return false;
            GoalSnapshot state;
            if (!nextLine(input, "ID ", payload) || !parseString(payload, state.id)) return false;
            if (!nextLine(input, "NAME ", payload) || !parseString(payload, state.name)) return false;
            if (!nextLine(input, "DESCRIPTION ", payload) || !parseString(payload, state.description)) return false;
            boundedNonempty(state.id, Goal::MAX_ID_LENGTH, "goal id");
            boundedNonempty(state.name, Goal::MAX_NAME_LENGTH, "goal name");
            bounded(state.description, Goal::MAX_DESCRIPTION_LENGTH, "goal description");

            if (!nextLine(input, "STATE ", payload)) return false;
            {
                int status = 0, priority = 0, type = 0;
                std::istringstream row(payload);
                if (!(row >> status >> priority >> type >> state.progress)) return false;
                row >> std::ws;
                if (!row.eof() || status < 0 || status > 6 || priority < 0 ||
                    priority > 4 || type < 0 || type > 4 || !std::isfinite(state.progress) ||
                    state.progress < 0.0 || state.progress > 1.0) return false;
                state.status = static_cast<GoalStatus>(status);
                state.priority = static_cast<GoalPriority>(priority);
                state.type = static_cast<GoalType>(type);
            }

            if (!nextLine(input, "TIMES ", payload)) return false;
            {
                long long created = 0, updated = 0, completed = 0, deadline = 0;
                int hasCompleted = 0, hasDeadline = 0;
                std::istringstream row(payload);
                if (!(row >> created >> updated >> hasCompleted >> completed >> hasDeadline >> deadline)) return false;
                row >> std::ws;
                if (!row.eof() || (hasCompleted != 0 && hasCompleted != 1) ||
                    (hasDeadline != 0 && hasDeadline != 1)) return false;
                state.createdAt = timestampFromNanos(created);
                state.updatedAt = timestampFromNanos(updated);
                if (hasCompleted) state.completedAt = timestampFromNanos(completed);
                if (hasDeadline) state.deadline = timestampFromNanos(deadline);
            }

            if (!nextLine(input, "EFFORT ", payload)) return false;
            {
                std::istringstream row(payload);
                if (!(row >> state.reward >> state.estimatedEffort >> state.actualEffort)) return false;
                row >> std::ws;
                if (!row.eof() || !std::isfinite(state.reward) ||
                    !std::isfinite(state.estimatedEffort) || !std::isfinite(state.actualEffort) ||
                    state.reward < 0.0 || state.estimatedEffort <= 0.0 || state.actualEffort < 0.0)
                    return false;
            }

            auto readStringList = [&](const char* countPrefix, const char* itemPrefix,
                                     std::size_t maximum, std::vector<std::string>& values) {
                if (!nextLine(input, countPrefix, payload)) return false;
                std::size_t count = 0;
                if (!parseSize(payload, count) || count > maximum) return false;
                values.reserve(count);
                for (std::size_t item = 0; item < count; ++item) {
                    std::string value;
                    if (!nextLine(input, itemPrefix, payload) || !parseString(payload, value)) return false;
                    values.push_back(std::move(value));
                }
                return true;
            };
            if (!readStringList("DEPS ", "DEP ", Goal::MAX_DEPENDENCY_COUNT, state.dependencies)) return false;
            if (!readStringList("SUBS ", "SUB ", kMaxSerializedGoals, state.subGoalIds)) return false;
            if (!readStringList("TAGS ", "TAG ", Goal::MAX_TAG_COUNT, state.tags)) return false;
            for (const auto& dependency : state.dependencies)
                boundedNonempty(dependency, Goal::MAX_ID_LENGTH, "dependency id");
            for (const auto& subGoal : state.subGoalIds)
                boundedNonempty(subGoal, Goal::MAX_ID_LENGTH, "sub-goal id");
            for (const auto& tag : state.tags)
                boundedNonempty(tag, Goal::MAX_TAG_LENGTH, "tag");

            if (!nextLine(input, "METADATA ", payload)) return false;
            std::size_t metadataCount = 0;
            if (!parseSize(payload, metadataCount) || metadataCount > Goal::MAX_METADATA_COUNT) return false;
            for (std::size_t item = 0; item < metadataCount; ++item) {
                if (!nextLine(input, "META ", payload)) return false;
                std::string key, value;
                if (encodedStrings) {
                    std::string encodedKey, encodedValue;
                    std::istringstream row(payload);
                    if (!(row >> encodedKey >> encodedValue) || !hexValue(encodedKey, key) ||
                        !hexValue(encodedValue, value)) return false;
                    row >> std::ws;
                    if (!row.eof()) return false;
                } else {
                    std::istringstream row(payload);
                    if (!(row >> std::quoted(key) >> std::quoted(value))) return false;
                    row >> std::ws;
                    if (!row.eof()) return false;
                }
                boundedNonempty(key, Goal::MAX_METADATA_KEY_LENGTH, "metadata key");
                bounded(value, Goal::MAX_METADATA_VALUE_LENGTH, "metadata value");
                state.metadata.emplace_back(std::move(key), std::move(value));
            }
            if (!std::getline(input, line) || line != "END") return false;
            states.push_back(std::move(state));
        }
        input >> std::ws;
        if (!input.eof()) return false;

        GoalMap parsed;
        for (const auto& state : states) {
            if (state.updatedAt < state.createdAt ||
                (state.deadline && *state.deadline < state.createdAt) ||
                (state.completedAt && *state.completedAt < state.createdAt) ||
                (state.status == GoalStatus::COMPLETED) != state.completedAt.has_value() ||
                (state.status == GoalStatus::COMPLETED && state.progress != 1.0)) return false;
            if (std::adjacent_find(state.dependencies.begin(), state.dependencies.end()) != state.dependencies.end() ||
                std::adjacent_find(state.subGoalIds.begin(), state.subGoalIds.end()) != state.subGoalIds.end() ||
                std::adjacent_find(state.tags.begin(), state.tags.end()) != state.tags.end()) return false;
            auto goal = std::make_shared<Goal>(state.id, state.name, state.description);
            {
                std::lock_guard<std::mutex> goalLock(goal->stateMutex_);
                goal->status_ = state.status;
                goal->priority_ = state.priority;
                goal->type_ = state.type;
                goal->progress_ = state.progress;
                goal->deadline_ = state.deadline;
                goal->createdAt_ = state.createdAt;
                goal->updatedAt_ = state.updatedAt;
                goal->completedAt_ = state.completedAt;
                goal->reward_ = state.reward;
                goal->estimatedEffort_ = state.estimatedEffort;
                goal->actualEffort_ = state.actualEffort;
                for (const auto& dependency : state.dependencies)
                    if (!goal->dependencies_.insert(dependency).second) return false;
                for (const auto& tag : state.tags)
                    if (!goal->tags_.insert(tag).second) return false;
                for (const auto& item : state.metadata)
                    if (!goal->metadata_.emplace(item).second) return false;
            }
            if (!parsed.emplace(state.id, std::move(goal)).second) return false;
        }

        std::unordered_map<UUID, UUID> parentOf;
        for (const auto& state : states) {
            auto goal = parsed.at(state.id);
            for (const auto& dependency : state.dependencies) {
                if (dependency == state.id || !parsed.count(dependency)) return false;
            }
            for (const auto& subGoalId : state.subGoalIds) {
                if (subGoalId == state.id || !parsed.count(subGoalId) ||
                    !parentOf.emplace(subGoalId, state.id).second) return false;
                goal->subGoals_.push_back(parsed.at(subGoalId));
            }
        }
        for (const auto& parent : parentOf) parsed.at(parent.first)->parentGoal_ = parsed.at(parent.second);

        auto graphHasCycle = [&](bool hierarchy) {
            enum class Mark { VISITING, DONE };
            std::unordered_map<UUID, Mark> marks;
            std::function<bool(const UUID&)> visit = [&](const UUID& id) {
                auto mark = marks.find(id);
                if (mark != marks.end()) return mark->second == Mark::VISITING;
                marks[id] = Mark::VISITING;
                const auto& state = *std::find_if(states.begin(), states.end(),
                    [&](const GoalSnapshot& candidate) { return candidate.id == id; });
                const auto& edges = hierarchy ? state.subGoalIds : state.dependencies;
                for (const auto& edge : edges) if (visit(edge)) return true;
                marks[id] = Mark::DONE;
                return false;
            };
            for (const auto& state : states) if (visit(state.id)) return true;
            return false;
        };
        if (graphHasCycle(false) || graphHasCycle(true)) return false;

        std::lock_guard<std::mutex> lock(mutex_);
        goals_.swap(parsed);
        for (const auto& entry : goals_) observeRestoredId(entry.first);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

} // namespace elizaos

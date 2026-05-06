#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-gamification/src/schema.h"

any gamificationEventsTable = pgTable(std::string("gamification_events"), object{
    object::pair{std::string("eventId"), uuid(std::string("event_id"))->primaryKey()->defaultRandom()}, 
    object::pair{std::string("userId"), uuid(std::string("user_id"))->notNull()}, 
    object::pair{std::string("actionType"), text(std::string("action_type"))->notNull()}, 
    object::pair{std::string("points"), integer(std::string("points"))->notNull()}, 
    object::pair{std::string("metadata"), jsonb(std::string("metadata"))}, 
    object::pair{std::string("sourceEventId"), uuid(std::string("source_event_id"))}, 
    object::pair{std::string("createdAt"), timestamp(std::string("created_at"))->notNull()->defaultNow()}
}, [=](auto table) mutable
{
    return (object{
        object::pair{std::string("userIdIdx"), index(std::string("gamification_events_user_id_idx"))->on(table["userId"])}, 
        object::pair{std::string("createdAtIdx"), index(std::string("gamification_events_created_at_idx"))->on(table["createdAt"])}, 
        object::pair{std::string("actionTypeIdx"), index(std::string("gamification_events_action_type_idx"))->on(table["actionType"])}, 
        object::pair{std::string("userActionDateIdx"), index(std::string("gam_events_user_action_date_idx"))->on(table["userId"], table["actionType"], table["createdAt"])}
    });
}
);
any pointBalancesTable = pgTable(std::string("point_balances"), object{
    object::pair{std::string("userId"), uuid(std::string("user_id"))->primaryKey()}, 
    object::pair{std::string("allTimePoints"), integer(std::string("all_time_points"))->notNull()->default(0)}, 
    object::pair{std::string("weeklyPoints"), integer(std::string("weekly_points"))->notNull()->default(0)}, 
    object::pair{std::string("streakDays"), integer(std::string("streak_days"))->notNull()->default(0)}, 
    object::pair{std::string("lastLoginDate"), timestamp(std::string("last_login_date"))}, 
    object::pair{std::string("level"), integer(std::string("level"))->notNull()->default(0)}, 
    object::pair{std::string("updatedAt"), timestamp(std::string("updated_at"))->notNull()->defaultNow()}
}, [=](auto table) mutable
{
    return (object{
        object::pair{std::string("allTimePointsIdx"), index(std::string("point_balances_all_time_points_idx"))->on(table["allTimePoints"])}, 
        object::pair{std::string("weeklyPointsIdx"), index(std::string("point_balances_weekly_points_idx"))->on(table["weeklyPoints"])}
    });
}
);
any referralCodesTable = pgTable(std::string("referral_codes"), object{
    object::pair{std::string("userId"), uuid(std::string("user_id"))->primaryKey()}, 
    object::pair{std::string("code"), text(std::string("code"))->notNull()->unique()}, 
    object::pair{std::string("referrerId"), uuid(std::string("referrer_id"))}, 
    object::pair{std::string("status"), text(std::string("status"))->notNull()->default(std::string("active"))}, 
    object::pair{std::string("createdAt"), timestamp(std::string("created_at"))->notNull()->defaultNow()}
}, [=](auto table) mutable
{
    return (object{
        object::pair{std::string("codeIdx"), index(std::string("referral_codes_code_idx"))->on(table["code"])}, 
        object::pair{std::string("referrerIdIdx"), index(std::string("referral_codes_referrer_id_idx"))->on(table["referrerId"])}
    });
}
);
any userChainHistoryTable = pgTable(std::string("user_chain_history"), object{
    object::pair{std::string("userId"), uuid(std::string("user_id"))->notNull()}, 
    object::pair{std::string("chain"), text(std::string("chain"))->notNull()}, 
    object::pair{std::string("firstTxAt"), timestamp(std::string("first_tx_at"))->notNull()->defaultNow()}
}, [=](auto table) mutable
{
    return (object{
        object::pair{std::string("userIdChainIdx"), index(std::string("user_chain_history_user_id_chain_idx"))->on(table["userId"], table["chain"])}, 
        object::pair{std::string("userIdIdx"), index(std::string("user_chain_history_user_id_idx"))->on(table["userId"])}
    });
}
);
any gamificationCampaignsTable = pgTable(std::string("gamification_campaigns"), object{
    object::pair{std::string("id"), uuid(std::string("id"))->primaryKey()->defaultRandom()}, 
    object::pair{std::string("name"), text(std::string("name"))->notNull()}, 
    object::pair{std::string("actionType"), text(std::string("action_type"))}, 
    object::pair{std::string("multiplier"), integer(std::string("multiplier"))->notNull()}, 
    object::pair{std::string("startAt"), timestamp(std::string("start_at"))->notNull()}, 
    object::pair{std::string("endAt"), timestamp(std::string("end_at"))->notNull()}, 
    object::pair{std::string("active"), boolean(std::string("active"))->notNull()->default(true)}, 
    object::pair{std::string("createdAt"), timestamp(std::string("created_at"))->notNull()->defaultNow()}
}, [=](auto table) mutable
{
    return (object{
        object::pair{std::string("activeIdx"), index(std::string("gamification_campaigns_active_idx"))->on(table["active"])}, 
        object::pair{std::string("actionTypeIdx"), index(std::string("gamification_campaigns_action_type_idx"))->on(table["actionType"])}
    });
}
);
any leaderboardSnapshotsTable = pgTable(std::string("leaderboard_snapshots"), object{
    object::pair{std::string("scope"), text(std::string("scope"))->notNull()}, 
    object::pair{std::string("rank"), integer(std::string("rank"))->notNull()}, 
    object::pair{std::string("userId"), uuid(std::string("user_id"))->notNull()}, 
    object::pair{std::string("points"), integer(std::string("points"))->notNull()}, 
    object::pair{std::string("snapshotAt"), timestamp(std::string("snapshot_at"))->notNull()->defaultNow()}
}, [=](auto table) mutable
{
    return (object{
        object::pair{std::string("scopeRankIdx"), index(std::string("leaderboard_snapshots_scope_rank_idx"))->on(table["scope"], table["rank"])}, 
        object::pair{std::string("scopePointsIdx"), index(std::string("leaderboard_snapshots_scope_points_idx"))->on(table["scope"], table["points"])}
    });
}
);
object gamificationSchema = object{
    object::pair{std::string("gamificationEventsTable"), std::string("gamificationEventsTable")}, 
    object::pair{std::string("pointBalancesTable"), std::string("pointBalancesTable")}, 
    object::pair{std::string("referralCodesTable"), std::string("referralCodesTable")}, 
    object::pair{std::string("userChainHistoryTable"), std::string("userChainHistoryTable")}, 
    object::pair{std::string("gamificationCampaignsTable"), std::string("gamificationCampaignsTable")}, 
    object::pair{std::string("leaderboardSnapshotsTable"), std::string("leaderboardSnapshotsTable")}
};

void Main(void)
{
}

MAIN

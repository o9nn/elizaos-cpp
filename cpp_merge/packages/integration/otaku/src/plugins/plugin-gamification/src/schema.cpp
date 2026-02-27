#include "schema.hpp"

any gamificationEventsTable = pgTable(std:("gamification_events"), object{
    object::pair{std:("eventId"), uuid(std:("event_id"))->primaryKey()->defaultRandom()}, 
    object::pair{std:("userId"), uuid(std:("user_id"))->notNull()}, 
    object::pair{std:("actionType"), text(std:("action_type"))->notNull()}, 
    object::pair{std:("points"), integer(std:("points"))->notNull()}, 
    object::pair{std:("metadata"), jsonb(std:("metadata"))}, 
    object::pair{std:("sourceEventId"), uuid(std:("source_event_id"))}, 
    object::pair{std:("createdAt"), timestamp(std:("created_at"))->notNull()->defaultNow()}
}, [=](auto table) mutable
{
    return (object{
        object::pair{std:("userIdIdx"), index(std:("gamification_events_user_id_idx"))->on(table["userId"])}, 
        object::pair{std:("createdAtIdx"), index(std:("gamification_events_created_at_idx"))->on(table["createdAt"])}, 
        object::pair{std:("actionTypeIdx"), index(std:("gamification_events_action_type_idx"))->on(table["actionType"])}, 
        object::pair{std:("userActionDateIdx"), index(std:("gam_events_user_action_date_idx"))->on(table["userId"], table["actionType"], table["createdAt"])}
    });
}
);
any pointBalancesTable = pgTable(std:("point_balances"), object{
    object::pair{std:("userId"), uuid(std:("user_id"))->primaryKey()}, 
    object::pair{std:("allTimePoints"), integer(std:("all_time_points"))->notNull()->default(0)}, 
    object::pair{std:("weeklyPoints"), integer(std:("weekly_points"))->notNull()->default(0)}, 
    object::pair{std:("streakDays"), integer(std:("streak_days"))->notNull()->default(0)}, 
    object::pair{std:("lastLoginDate"), timestamp(std:("last_login_date"))}, 
    object::pair{std:("level"), integer(std:("level"))->notNull()->default(0)}, 
    object::pair{std:("updatedAt"), timestamp(std:("updated_at"))->notNull()->defaultNow()}
}, [=](auto table) mutable
{
    return (object{
        object::pair{std:("allTimePointsIdx"), index(std:("point_balances_all_time_points_idx"))->on(table["allTimePoints"])}, 
        object::pair{std:("weeklyPointsIdx"), index(std:("point_balances_weekly_points_idx"))->on(table["weeklyPoints"])}
    });
}
);
any referralCodesTable = pgTable(std:("referral_codes"), object{
    object::pair{std:("userId"), uuid(std:("user_id"))->primaryKey()}, 
    object::pair{std:("code"), text(std:("code"))->notNull()->unique()}, 
    object::pair{std:("referrerId"), uuid(std:("referrer_id"))}, 
    object::pair{std:("status"), text(std:("status"))->notNull()->default(std:("active"))}, 
    object::pair{std:("createdAt"), timestamp(std:("created_at"))->notNull()->defaultNow()}
}, [=](auto table) mutable
{
    return (object{
        object::pair{std:("codeIdx"), index(std:("referral_codes_code_idx"))->on(table["code"])}, 
        object::pair{std:("referrerIdIdx"), index(std:("referral_codes_referrer_id_idx"))->on(table["referrerId"])}
    });
}
);
any userChainHistoryTable = pgTable(std:("user_chain_history"), object{
    object::pair{std:("userId"), uuid(std:("user_id"))->notNull()}, 
    object::pair{std:("chain"), text(std:("chain"))->notNull()}, 
    object::pair{std:("firstTxAt"), timestamp(std:("first_tx_at"))->notNull()->defaultNow()}
}, [=](auto table) mutable
{
    return (object{
        object::pair{std:("userIdChainIdx"), index(std:("user_chain_history_user_id_chain_idx"))->on(table["userId"], table["chain"])}, 
        object::pair{std:("userIdIdx"), index(std:("user_chain_history_user_id_idx"))->on(table["userId"])}
    });
}
);
any gamificationCampaignsTable = pgTable(std:("gamification_campaigns"), object{
    object::pair{std:("id"), uuid(std:("id"))->primaryKey()->defaultRandom()}, 
    object::pair{std:("name"), text(std:("name"))->notNull()}, 
    object::pair{std:("actionType"), text(std:("action_type"))}, 
    object::pair{std:("multiplier"), integer(std:("multiplier"))->notNull()}, 
    object::pair{std:("startAt"), timestamp(std:("start_at"))->notNull()}, 
    object::pair{std:("endAt"), timestamp(std:("end_at"))->notNull()}, 
    object::pair{std:("active"), boolean(std:("active"))->notNull()->default(true)}, 
    object::pair{std:("createdAt"), timestamp(std:("created_at"))->notNull()->defaultNow()}
}, [=](auto table) mutable
{
    return (object{
        object::pair{std:("activeIdx"), index(std:("gamification_campaigns_active_idx"))->on(table["active"])}, 
        object::pair{std:("actionTypeIdx"), index(std:("gamification_campaigns_action_type_idx"))->on(table["actionType"])}
    });
}
);
any leaderboardSnapshotsTable = pgTable(std:("leaderboard_snapshots"), object{
    object::pair{std:("scope"), text(std:("scope"))->notNull()}, 
    object::pair{std:("rank"), integer(std:("rank"))->notNull()}, 
    object::pair{std:("userId"), uuid(std:("user_id"))->notNull()}, 
    object::pair{std:("points"), integer(std:("points"))->notNull()}, 
    object::pair{std:("snapshotAt"), timestamp(std:("snapshot_at"))->notNull()->defaultNow()}
}, [=](auto table) mutable
{
    return (object{
        object::pair{std:("scopeRankIdx"), index(std:("leaderboard_snapshots_scope_rank_idx"))->on(table["scope"], table["rank"])}, 
        object::pair{std:("scopePointsIdx"), index(std:("leaderboard_snapshots_scope_points_idx"))->on(table["scope"], table["points"])}
    });
}
);
object gamificationSchema = object{
    object::pair{std:("gamificationEventsTable"), std:("gamificationEventsTable")}, 
    object::pair{std:("pointBalancesTable"), std:("pointBalancesTable")}, 
    object::pair{std:("referralCodesTable"), std:("referralCodesTable")}, 
    object::pair{std:("userChainHistoryTable"), std:("userChainHistoryTable")}, 
    object::pair{std:("gamificationCampaignsTable"), std:("gamificationCampaignsTable")}, 
    object::pair{std:("leaderboardSnapshotsTable"), std:("leaderboardSnapshotsTable")}
};

void Main(void)
{
}

MAIN

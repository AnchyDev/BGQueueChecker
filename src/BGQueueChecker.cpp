#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"

class BGQPlayerScript : public PlayerScript
{
public:
    BGQPlayerScript() : PlayerScript("BGQPlayerScript") { }

    bool CanJoinInBattlegroundQueue(Player* player, ObjectGuid /*BattlemasterGuid*/, BattlegroundTypeId /*BGTypeID*/, uint8 joinAsGroup, GroupJoinBattlegroundResult& err) override
    {
        if (!sConfigMgr->GetOption<bool>("BGQueueChecker.Enable", false))
        {
            return true;
        }

        if (!joinAsGroup || !player)
        {
            return true;
        }

        auto group = player->GetGroup();
        if (!group)
        {
            return true;
        }

        bool sameFaction = true;
        uint32 leaderFaction = player->GetFaction();

        for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
        {
            Player* member = itr->GetSource();
            if (!member)
            {
                continue;
            }

            if (member->GetFaction() != leaderFaction)
            {
                sameFaction = false;
            }
        }

        if (!sameFaction)
        {
            err = GroupJoinBattlegroundResult::ERR_BATTLEGROUND_JOIN_FAILED;
            player->SendSystemMessage("Unable to join battleground queue. All party members must be the same faction as the party leader.");
        }

        return sameFaction;
    }
};

void SC_AddBGQueueCheckerScripts()
{
    new BGQPlayerScript();
}

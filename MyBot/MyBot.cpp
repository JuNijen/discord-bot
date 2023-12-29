#include "MyBot.h"
#include <cstdio>
#include <dpp/dpp.h>
#include <iomanip>
#include <sstream>

int main() {
    /* Example to record a user in a VC
    *
    * Recording is output as './me.pcm' and you can play it via the soundboard example
    * or use ffmpeg 'ffplay -f s16le -ar 48000 -ac 2 -i ./me.pcm'
    */

    /* Replace with the user's id you wish to record */
    dpp::snowflake user_id = 1190197595453272104;

    /* Setup the bot */
    dpp::cluster bot("MTE5MDE5NzU5NTQ1MzI3MjEwNA.GfmuIJ.PYG-8ZfB21WTXKfEdCXwfsgXOVzftvhFcK61jg");

    FILE* fd;
    fopen_s(&fd, "./me.pcm", "wb");

    bot.on_log(dpp::utility::cout_logger());

    /* The event is fired when someone issues your commands */
    bot.on_slashcommand([&bot, &fd](const dpp::slashcommand_t& event) {
        /* Check which command they ran */
        if (event.command.get_command_name() == "record") {
            /* Get the guild */
            dpp::guild* g = dpp::find_guild(event.command.guild_id);

            /* Attempt to connect to a voice channel, returns false if we fail to connect. */
            if (!g->connect_member_voice(event.command.get_issuing_user().id)) {
                event.reply("보이스 채널에 참가중이지 않습니다.");
                return;
            }

            /* Tell the user we joined their channel. */
            event.reply("음성 채널에 참여하였습니다. 녹음을 시작합니다.");
        }
        else if (event.command.get_command_name() == "stop") {
            event.from->disconnect_voice(event.command.guild_id);
            fclose(fd);

            event.reply("녹음을 종료합니다.");
        }
        });

    bot.on_voice_receive([&bot, &fd, &user_id](const dpp::voice_receive_t& event) {
        if (event.user_id == user_id) {
            fwrite((char*)event.audio, 1, event.audio_size, fd);
        }
        });

    bot.on_ready([&bot](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            /* Create a new command. */
            dpp::slashcommand recordcommand("record", "음성 채널에 참여하여 녹음합니다.", bot.me.id);
            dpp::slashcommand stopcommand("stop", "녹음을 종료합니다.", bot.me.id);

            bot.global_bulk_command_create({ recordcommand, stopcommand });
        }
        });

    /* Start bot */
    bot.start(dpp::st_wait);

    return 0;
}
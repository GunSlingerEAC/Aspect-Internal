
#include "../../commons.h"
#include "../../sdk/sdk.h"
#include "../../renderer/renderer.h"
#include "../../settings.h"
#include "../../security/security.h"
#include "../../global.h"
#include "aimbot.h"

extern struct sdk_t g_sdk;
extern struct settings_t g_settings;
extern struct security_t g_security;

void aimbot_t::update()
{
	if (!g_security.authenticated)
		return;
	if (!g_settings.aim.enabled)
		return;

	if (g_settings.aim.key > 0 && g_input.key_pressed[g_settings.aim.key])
	{
		target_t target;

		for (auto child : *g_sdk.players->children)
		{
			auto local_player = g_sdk.players->get_local_player();
			if (child->user_id == local_player->user_id)
				continue;
			if (child->team_id == local_player->team_id && g_settings.aim.team_check)
				continue;

			auto local_character = local_player->character;
			if (INSTANCE_CHECK(local_character))
				continue;

			auto local_head = local_character->find_child<RBXInstance>("Head");
			if (INSTANCE_CHECK(local_head))
				continue;

			auto character = child->character;
			if (INSTANCE_CHECK(character))
				continue;

			auto head = character->find_child<RBXInstance>("Head");
			if (INSTANCE_CHECK(head))
				continue;

			vec2 sc;
			vec3 target_pos = head->get_primitive()->get_body()->get_position();
			vec3 local_pos = local_head->get_primitive()->get_body()->get_position();
			if (g_settings.aim.head)
				target_pos.y += 1.5f;

			float pdistance = g_sdk.distance_to<vec3>(local_pos, target_pos);

			if (g_renderer.w2s(target_pos, sc))
			{
				float distance = this->distance_cross(sc);
				if (distance < target.dist && distance <= g_settings.aim.fov && pdistance <= g_settings.aim.distance)
					target = target_t { character->name, sc, distance };
			}
		}
		if (target.sc.x != 1 && target.sc.y != 1)
			this->aim_at(
				vec2{ target.sc.x, target.sc.y } ),
			global.target = target;
	}
	else {
		global.target = target_t();
	}
}

void aimbot_t::aim_at(vec2 pos)
{
    vec2 center { g_renderer.s_w / 2, g_renderer.s_h / 2 };
    vec2 target { 0, 0 };
    if (pos.x != 0) {
        if (pos.x > center.x) {
            target.x = -(center.x - pos.x);
            target.x /= 1;
            if (target.x + center.x > center.x * 2)
                target.x = 0;
        }

        if (pos.x < center.x) {
            target.x = pos.x - center.x;
            target.x /= 1;
            if (target.x + center.x < 0)
                target.x = 0;
        }
    }

    if (pos.y != 0) {
        if (pos.y > center.y) {
            target.y = -(center.y - pos.y);
            target.y /= 1;
            if (target.y + center.y > center.y * 2)
                target.y = 0;
        }

        if (pos.y < center.y) {
            target.y = pos.y - center.y;
            target.y /= 1;
            if (target.y + center.y < 0)
                target.y = 0;
        }
    }

    target.x /= g_settings.aim.smooth;
    target.y /= g_settings.aim.smooth;

    if (abs(target.x) < 1) {
        if (target.x > 0) {
            target.x = 1;
        }
        if (target.x < 0) {
            target.x = -1;
        }
    }
    if (abs(target.y) < 1) {
        if (target.y > 0) {
            target.y = 1;
        }
        if (target.y < 0) {
            target.y = -1;
        }
    }

    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.mouseData = 0;
    input.mi.time = 0;
    input.mi.dx = target.x + g_settings.aim.x_off;
    input.mi.dy = target.y;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    SendInput(1, &input, sizeof(input));
}

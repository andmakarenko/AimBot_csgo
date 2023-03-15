#include "memory.h"
#include "vector.h"
#include <thread>
//#include "val.h"
#include "CSGOoffsets.h"

constexpr Vector CalculateAngle(
	const Vector& localPos,
	const Vector& enemyPos,
	const Vector& viewAngles) noexcept
{
	return ((enemyPos - localPos).ToAngle() - viewAngles);
}

int main()
{
	const auto memory = Memory{ "csgo.exe" };

	const auto client = memory.GetModuleAddress("client.dll");
	const auto engine = memory.GetModuleAddress("engine.dll");



	while (true)
	{
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));

		if (!GetAsyncKeyState(VK_RBUTTON))
			continue;

		const auto& localPlayer = memory.Read<std::uintptr_t>(client + offsets::dwLocalPlayer);
		const auto& localTeam = memory.Read<std::int32_t>(localPlayer + offsets::m_iTeamNum);

		const auto localEyePos = memory.Read<Vector>(localPlayer + offsets::m_vecOrigin) + memory.Read<Vector>(localPlayer + offsets::m_vecViewOffset);

		const auto& clientState = memory.Read<std::uintptr_t>(engine + offsets::dwClientState);

		const auto& viewAngles = memory.Read<Vector>(clientState + offsets::dwClientState_ViewAngles);
		const auto& aimPunch = memory.Read<Vector>(localPlayer + offsets::m_aimPunchAngle) * 2;

		auto bestFov = 5.f;
		auto bestAngle = Vector{};

		for (auto i = 1; i <= 32; i++)
		{
			const auto& player = memory.Read<std::uintptr_t>(client + offsets::dwEntityList + i * 0x10);

			if (memory.Read<std::int32_t>(player + offsets::m_iTeamNum) == localTeam)
				continue;

			if (memory.Read<bool>(player + offsets::m_bDormant))
				continue;

			if (!memory.Read<std::int32_t>(player + offsets::m_iHealth))
				continue;

			if (!memory.Read<bool>(player + offsets::m_bSpottedByMask))
				continue;

			const auto boneMatrix = memory.Read<std::uintptr_t>(player + offsets::m_dwBoneMatrix);

			const auto playerHeadPos = Vector{
				memory.Read<float>(boneMatrix + 0x30 * 8 + 0x0C),
				memory.Read<float>(boneMatrix + 0x30 * 8 + 0x1C),
				memory.Read<float>(boneMatrix + 0x30 * 8 + 0x2C)
			};

			const auto angle = CalculateAngle(
				localEyePos,
				playerHeadPos,
				viewAngles + aimPunch
			);

			const auto fov = std::hypot(angle.x, angle.y);

			if (fov < bestFov)
			{
				bestFov = fov;
				bestAngle = angle;
			}

		}

		if (!bestAngle.IsZero())
			memory.Write<Vector>(clientState + offsets::dwClientState_ViewAngles, viewAngles + bestAngle);

	}

	return 0;
}
#pragma once
#include "lua.hpp"
#include "luapromise.h"
#include "mysql.h"
#include "luadatabase.h"

namespace gluamysql {
	class LuaAction : public LuaPromise {
	public:
		LuaAction(lua_State* L) : LuaPromise{ L } {

		}

		virtual bool Query(lua_State *L, LuaDatabase *db) = 0;
		virtual void Finish(lua_State *L, LuaDatabase *db) = 0;

		// Ran by Tick
		void DoFinish(lua_State* L, LuaDatabase* db) {
			if (has_finished) {
				return;
			}
			has_finished = true;
			Finish(L, db);
		}

		void Free(lua_State *L) override {
			LuaPromise::Free(L);
		}

		void Reject(lua_State *L, LuaDatabase* db, const char *error = nullptr) {
			if (error == nullptr) {
				error = mysql_error(db->instance);
			}
			PushReject(L);
			lua_pushstring(L, error);
			lua_pushnumber(L, mysql_errno(db->instance));
			lua_call(L, 2, 0);
		}

	public:
		bool has_finished = false;
	};
}
#include "nomake.h"

/*
struct {
	sqlite3 *pDb;
} db;
*/


//struct sqlite3_api_routines *sqlite3_api;

struct {
	sqlite3 *pDb;
} GLOBAL;

sqlite3 *pDb;

int db_run_nonquery(const char *sql)
{
	int result;
	sqlite3_stmt *pStmt;
	const char *nxtQuery;

	nxtQuery = sql;

	do {
		result = winsqlite3.prepare_v2(GLOBAL.pDb, nxtQuery, -1, &pStmt, &nxtQuery);
		if(result != SQLITE_OK || !pStmt)
		{
			goto on_error;	
		}

		result = winsqlite3.step(pStmt);
		if(result != SQLITE_DONE)
		{
			goto on_error;	
		}

		result = winsqlite3.finalize(pStmt);
		if(result != SQLITE_OK)
		{
			goto on_error;	
		}
	} while(nxtQuery && nxtQuery[0] != '\0');

	return 1;

on_error:
	printf("Error: %s : %s\n",
		winsqlite3.errstr(result), winsqlite3.errmsg(pDb));
	return 0;
}


int db_create_tables()
{
	const char *sql =
	"BEGIN TRANSACTION;"

	"CREATE TABLE IF NOT EXISTS project_type("
	"id INTEGER NOT NULL,"
	"name TEXT NOT NULL,"
	"desc TEXT NULL,"
	"template_path TEXT NULL,"
	"PRIMARY KEY(id ASC)"
	");"

	"CREATE TABLE IF NOT EXISTS project("
	"id INTEGER NOT NULL,"
	"name TEXT NOT NULL,"
	"project_type_id INTEGER NOT NULL,"
	"version_control_url TEXT NULL,"
	"uuid blob not null," // the 16 byte (128 bit) id abstraction
	"create_time INTEGER NOT NULL,"
	//"last_update_time INTEGER NOT NULL,"
	"FOREIGN KEY(project_type_id) REFERENCES project_type(id),"
	"PRIMARY KEY(id ASC)"
	");"

	"CREATE TABLE IF NOT EXISTS build("
	"id INTEGER NOT NULL,"
	"project_id INTEGER NOT NULL,"
	"create_time INTEGER NOT NULL,"
	"FOREIGN KEY(project_id) REFERENCES project(id),"
	"PRIMARY KEY (id ASC)"
	");"

	"create table if not exists source_units("
	"id integer not null,"
	"project_id integer not null,"
	//"file_id integer not null
	"hash blob not null," // the 20 byte (160 bit) sha1 hash
	"create_time INTEGER NOT NULL,"
	"FOREIGN KEY(project_id) REFERENCES project(id),"
	"PRIMARY KEY(id ASC)"
	");"

	"INSERT INTO project_type(name, desc)"
	"VALUES"
	"('c-windows', 'A Windows C project'),"
	"('c-linux', 'A Linux C project')"
	";"

	"COMMIT;"
	;

	return db_run_nonquery(sql);
}


int db_create_project_type_table()
{
	const char *sql =
	"CREATE TABLE IF NOT EXISTS project_type("
	"id INTEGER NOT NULL,"
	"name TEXT NOT NULL,"
	"desc TEXT NULL,"
	"template_path TEXT NULL,"
	"PRIMARY KEY(id ASC)"
	");"
	;

	return db_run_nonquery(sql);
}


int db_insert_project_type_records()
{
	const char *sql =
	"INSERT INTO project_type (name, template_path)"
	"VALUES ('c-project', 'A Windows C project')"
	;

	return db_run_nonquery(sql);
}


int db_select_project_type_id_byname(int *id, const char *name)
{
	int result;
	sqlite3_stmt *pStmt = 0;
	char *nxtQuery;

	const char *sql =
	"SELECT id FROM project_type WHERE name = ?"
	;

	result = winsqlite3.prepare_v2(GLOBAL.pDb, sql, -1, &pStmt, &nxtQuery);
	printf("prepare result: %u\n", result);
	if(result != SQLITE_OK || !pStmt){
		goto on_error;
	}

	result = winsqlite3.bind_text(pStmt, 1, name, -1, 0);
	printf("bind result: %u\n", result);
	if(result != SQLITE_OK){
		goto on_error;
	}

	result = winsqlite3.step(pStmt);
	printf("step result: %u\n", result);
	if(result != SQLITE_ROW){
		goto on_error;
	}

	*id = winsqlite3.column_int(pStmt, 0);
	printf("column result: %u\n", *id);
	if(!*id){
		goto on_error;
	}

	result = winsqlite3.finalize(pStmt);
	if(result != SQLITE_OK){
		goto on_error;
	}
	return 1;

on_error:

	printf("Error: %s : %s\n",
		winsqlite3.errstr(winsqlite3.errcode(pDb)),
		winsqlite3.errmsg(pDb));
	return 0;
}

int db_create_project_table()
{
	const char *sql =
	"CREATE TABLE IF NOT EXISTS project ( "
	"id INTEGER NOT NULL, "
	"name TEXT NOT NULL, "
	"project_type_id INTEGER NOT NULL, "
	"version_control_url TEXT NULL, "
	"uuid TEXT NOT NULL, "
	"create_time INTEGER NOT NULL, "
	//"FOREIGN KEY(project_type_id) REFERENCES project_type(id), "
	"PRIMARY KEY(id ASC)"
	");"
	;

	return db_run_nonquery(sql);
}

int db_create_build_table()
{
	const char *sql =
	"CREATE TABLE IF NOT EXISTS build("
	"id INTEGER NOT NULL,"
	"project_id INTEGER NOT NULL,"
	"create_time INTEGER NOT NULL,"
	"FOREIGN KEY(project_id) REFERENCES project(id),"
	"PRIMARY KEY (id ASC)"
	;

	return db_run_nonquery(sql);
}

int  db_delete_project_byid(int project_id)
{
	sqlite3_stmt *pStmt;
	char *nxtQuery;

	const char *sql =
	"DELETE FROM project WHERE project_id = ?"
	;

	winsqlite3.prepare_v2(GLOBAL.pDb, sql, -1, &pStmt, &nxtQuery);
	winsqlite3.bind_int(pStmt, 1, project_id);
	winsqlite3.step(pStmt);

	winsqlite3.finalize(pStmt);
	return 1;
}

int db_select_all_projects(struct project_record rec[], int *size)
{
	sqlite3_stmt *pStmt;
	char *nxtQuery;
	int result, i;
	const char *str;
	int numbytes;

	const char *sql = "SELECT * FROM project";

	result = winsqlite3.prepare_v2(GLOBAL.pDb, sql, -1, &pStmt, &nxtQuery);
	if(result != SQLITE_OK){
		goto on_error;
	}


	for(i = 0; i < *size; i++)
	{
		result = winsqlite3.step(pStmt);
		if(result != SQLITE_ROW) break;

		rec[i].id = winsqlite3.column_int(pStmt, 0);

		str = winsqlite3.column_text(pStmt, 1);
		numbytes = winsqlite3.column_bytes(pStmt, 1);
		memcpy(rec[i].name, str, numbytes);  

		rec[i].project_type_id = winsqlite3.column_int(pStmt, 2);
		rec[i].create_time = winsqlite3.column_int64(pStmt, 5);
	}

	*size = i;

	result = winsqlite3.finalize(pStmt);
	if(result != SQLITE_OK){
		goto on_error;	
	}

	return 1;
	
on_error:

	printf("Error: %d : %s\n", result, winsqlite3.errmsg(pDb));
	return 0;
}


int db_select_project_byname(struct project_record *rec, const char *name)
{
	sqlite3_stmt *pStmt;
	char *nxtQuery;
	int result;

	const char *sql =
	"SELECT id, name, project_type_id FROM project WHERE name = ?";

	printf("finding project ...\n");

	result = winsqlite3.prepare_v2(GLOBAL.pDb, sql, -1, &pStmt, &nxtQuery);
	printf("prepare result: %u\n", result);
	if(result != SQLITE_OK){
		goto on_error;
	}
	result = winsqlite3.bind_text(pStmt, 1, name, -1, 0);
	if(result != SQLITE_OK){	
		goto on_error;
	}
	result = winsqlite3.step(pStmt);
	if(result != SQLITE_ROW){
		goto on_error;
	}

	rec->id              = winsqlite3.column_int(pStmt, 0);
	//rec->name            = winsqlite3.column_text(pStmt, 1);
	rec->project_type_id = winsqlite3.column_int(pStmt, 2);

	winsqlite3.finalize(pStmt);
	return 1;

on_error:

	printf("Error: %d : %s\n", result, winsqlite3.errmsg(pDb));
	return 0;

}

int db_insert_project_record(
const char *name,
int project_type_id,
const char *url,
uuid_t *uuid)
{
	SYSTEMTIME curtime;
	int result;
	sqlite3_stmt *pStmt;
	char *nxtQuery;
	sqlite3_int64 timestamp = 0;

	const char *sql =
	"INSERT INTO project("
	"name,"
	"project_type_id,"
	"version_control_url,"
	"uuid,"
	"create_time"
	")"
	"VALUES (?, ?, ?, ?, ?)"
	;

	//should check if a project with the same name already exists
	

	result = winsqlite3.prepare(GLOBAL.pDb, sql, -1, &pStmt, &nxtQuery);
	if(result != SQLITE_OK){
		goto on_error;	
	}

	result = winsqlite3.bind_text(pStmt, 1, name, -1, SQLITE_STATIC);
	if(result != SQLITE_OK){
		goto on_error;
	}
	result = winsqlite3.bind_int(pStmt, 2, project_type_id);
	if(result != SQLITE_OK){
		goto on_error;	
	}
	result = winsqlite3.bind_text(pStmt, 3, url, -1, SQLITE_STATIC);
	if(result != SQLITE_OK){
		goto on_error;	
	}
	result = winsqlite3.bind_blob(pStmt, 4, uuid, 16, SQLITE_STATIC);
	if(result != SQLITE_OK){
		goto on_error;
	}

	GetSystemTime(&curtime);
	systime_to_unixtime_ms(&curtime, (__time64_t*)&timestamp);
	result = winsqlite3.bind_int64(pStmt, 5, timestamp);
	if(result != SQLITE_OK){
		goto on_error;	
	}

	result = winsqlite3.step(pStmt);
	printf("insert step result: %u\n", result);
	if(result != SQLITE_DONE){
		goto on_error;	
	}

	winsqlite3.finalize(pStmt);
	return 1;

on_error:
	printf("Error: %d : %s\n", result, winsqlite3.errmsg(pDb));
	return 0;
}

int db_update_project_name(int project_id, const char *name)
{
	sqlite3_stmt *pStmt;
	char *nxtQuery;

	const char *sql =
	"UPDATE project SET name = ? WHERE project_id = ?";

	winsqlite3.prepare(GLOBAL.pDb, sql, -1, &pStmt, &nxtQuery);
	winsqlite3.bind_text(pStmt, 1, name, -1, SQLITE_STATIC);
	winsqlite3.bind_int(pStmt, 2, project_id);

	winsqlite3.step(pStmt);

	winsqlite3.finalize(pStmt);
	return 1;
}

int db_insert_build_record(int project_id)
{
	SYSTEMTIME curtime;
	sqlite3_stmt *pStmt;
	char *nxtQuery;
	sqlite3_int64 timestamp = 0;

	const char * sql =
	"INSERT INTO build("
	"project_id,"
	"create_time"	
	")"
	"VALUES(?, ?)"
	;

	winsqlite3.prepare(GLOBAL.pDb, sql, -1, &pStmt, &nxtQuery);

	winsqlite3.bind_int(pStmt, 1, project_id);
	GetSystemTime(&curtime);
	systime_to_unixtime_ms(&curtime, (__time64_t*)&timestamp);
	winsqlite3.bind_int64(pStmt, 4, timestamp);

	winsqlite3.step(pStmt);

	winsqlite3.finalize(pStmt);
	return 1;
}

int db_select_last_build_record(struct build_record *rec, int project_id)
{
	int result;
	sqlite3_stmt *pStmt;
	char *nxtQuery;

	const char * sql =
	"SELECT * FROM build WHERE project_id = ?"
	" ORDER BY id DESC LIMIT 1;"
	;

	winsqlite3.prepare_v2(GLOBAL.pDb, sql, -1, &pStmt, &nxtQuery);
	winsqlite3.bind_int(pStmt, 1, project_id);

	result = winsqlite3.step(pStmt);

	rec->project_id = winsqlite3.column_int(pStmt, 1);
	rec->create_time = winsqlite3.column_int64(pStmt, 2);


	winsqlite3.finalize(pStmt);
	return 1;
}

int db_select_all_project_types(struct project_type_record *ptrec)
{
	int result;
	sqlite3_stmt *pStmt;
	const char *nxtQuery;

	const char *sql =
	"SELECT * FROM project_type;";

	nxtQuery = sql;

	assert(GLOBAL.pDb);
	result = winsqlite3.prepare_v2(GLOBAL.pDb, nxtQuery, -1, &pStmt, &nxtQuery);
	printf("result: %u\n", result);
	if(result != SQLITE_OK){
		goto ON_ERROR;
	}

	result = winsqlite3.step(pStmt);
	printf("result: %u\n", result);
	if(result != SQLITE_OK){
		goto ON_ERROR;	
	}

	winsqlite3.finalize(pStmt);
	return 1;

ON_ERROR:
	printf("Error: %s\n", winsqlite3.errstr(result));
	return 0;
}


void database_get_path(char path[32])
{
	memcpy(path, ".\\nomake.dat", 13);
}

static void db_create(const char *db_path)
{
	struct database *db = 0;
	int result;
	int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

	result = winsqlite3.open_v2(db_path, &GLOBAL.pDb, flags, 0);
	if(result != SQLITE_OK) {
		printf("Error: Could not create sqlite database\n");	
		return;
	}

	result = db_create_tables();
	if(!result) goto ON_ERROR;

	return;

ON_ERROR:
	printf("Failed to create database\n");
}

/*
bool db_exists()
{
	char db_path[32];

}
*/

void db_open()
{
	int flags = SQLITE_OPEN_READWRITE;
	int result;
	struct database *db = 0;
	char db_path[256];


	database_get_path(db_path);
	if(!system_file_exists(db_path)){
		db_create(db_path);
	}
	else {
		//printf("opening project...\n");
		result = winsqlite3.open_v2(db_path, &GLOBAL.pDb, flags, 0);	
		if(result != SQLITE_OK){
			printf("Error: Could not open sqlite database\n"); 
			return;
		}
	}
}

void db_close()
{
	winsqlite3.close(GLOBAL.pDb);
}

import sqlite3


###########
# system
###########
def get_db():
    db = sqlite3.connect("rover.db")
    return db


def get_cur():
    db = get_db()
    cur = db.cursor()
    return cur


###########
# read only
###########
def query_table(cur, table):
    id, code = -1, None
    query = 'select rowid, code from {} order by rowid desc limit 1'.format(
        table)
    data = cur.execute(query).fetchone()
    if data:
        id, code = data

    # print(rec)
    return id, code


###########
# write only
###########
def edit_table(table, code):
    db = get_db()
    cur = db.cursor()

    query = 'insert into {} (code) values ({})'.format(table, str(code))
    data = cur.execute(query)

    db.commit()
    cur.close()
    return cur.lastrowid


def close(con):
    con.close()

# cur = get_cur()

# rec = query_table(cur, 'auto_receive')
# print(rec)

# rec = query_table(cur, 'manual_receive')
# print(rec)

# r = edit_table('auto_send', '1112341')
# print(r)

import sqlite3
from flask import g

DATABASE = 'rover.db'

#reference webpage: Using SQLite 3 with Flask. (2010)https://flask.palletsprojects.com/en/2.0.x/patterns/sqlite3/
def get_db():
    db = getattr(g, '_database', None)
    if db is None:
        db = g._database = sqlite3.connect(
            DATABASE, timeout=10, isolation_level=None)
    return db





def query_db(query, args=(), one=False):
    db = get_db()
    cur = db.execute(query, args)
    rv = cur.fetchall()
    cur.close()
    # db.close()

    return (rv[0] if rv else None) if one else rv
    # return rv


def update_db(query, args=(), one=False):
    db = get_db()
    cur = db.execute(query, args)
    db.commit()
    cur.close()
    # db.close()
    return cur.lastrowid


def new_code(table, code, field='code'):
    query = '''
        insert into {} ({}) values ({})
    '''.format(table, field, str(code))

    return update_db(query)


def close_db():
    db = get_db()
    db.close()

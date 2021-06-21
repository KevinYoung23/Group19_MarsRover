import sqlite3
con = sqlite3.connect('rover.db')


cur = con.cursor()

# Create table
# cur.execute('''CREATE TABLE manual (to_rover text, from_rover text)''')

cur.execute('''CREATE TABLE manual_send (code text)''')
cur.execute('''CREATE TABLE obstacle (grid_id TEXT NOT NULL UNIQUE)''')
cur.execute('''CREATE TABLE manual_receive (code text)''')

cur.execute('''CREATE TABLE auto_send (code text)''')
cur.execute('''CREATE TABLE auto_receive (code text)''')
cur.execute('''CREATE TABLE prev (code text)''')

# Insert a row of data
# Save (commit) the changes
con.commit()

# We can also close the connection if we are done with it.
# Just be sure any changes have been committed or they will be lost.
con.close()

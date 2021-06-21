import socket
# from local_server import *
from db_server import *
# from control_rover import *
from flask import Flask, jsonify, request, render_template, session


app = Flask(__name__)

app.secret_key = b'_5#y2L"F4Q8z\n\xec]/'

# @app.route('/fromJtoP', methods=['GET', 'POST'])
# def hello():
#     # POST request
#     if request.method == 'POST':
#         print('Incoming..')
#         data = request.get_json()
#         print(data)  # parse as JSON
#         # call the rover

#         # go_forward(100)
#         # connect = Connection("new")
#         # conn = get_conn()
#         mx = '500'
#         my = '400'

#         session['max_x'] = mx
#         session['max_y'] = my
#         print(session['max_x'])

#         if data == "call m":
#             send(request.get_json())

#         return 'OK', 200

#     # GET request
#     else:
#         message = {'greeting': 'Hello Socket take 1!'}
#         return jsonify(message)  # serialize and use JSON headers


# @app.route('/roverdata')
# def rover_data():
#     # connect = Connection.create()
#     # connect = Connection("new")
#     # return "test"
#     # return "100"
#     d = recv()
#     print("ssssss::::")
#     print(d)
#     return d


# @app.route('/')
# def home():
#     return "test"
# return render_template('button.html')


@app.route('/set-obstacle/<grid_id>')
def set_o(grid_id):
    exist_check = 'select * from obstacle where grid_id = {}'.format(grid_id)
    rec = query_db(exist_check)
    # return jsonify(rec)
    if not rec:
        query = 'insert into {} (grid_id) values ("{}")'.format(
            'obstacle', grid_id)

        rec = update_db(query)

    return jsonify(rec)
    # return jsonify(query)


@app.route('/get-obstacle/')
def get_o():
    query = 'select grid_id from obstacle'
    obstacle = query_db(query)
    obs = [rec[0] for rec in obstacle]

    obj = {
        'data': {
            'obstacles': obs,
        }
    }

    return jsonify(obj)

###################
# manual
###################
# @app.route('/manual-send/<code>/<grid_id>')


@app.route('/done-update/<done_id>')
def done_update(done_id):
    session['done_id'] = done_id
    return jsonify(done_id)


@app.route('/manual-send/<code>')
def m_send(code):
    direction = code[3]
    rec = new_code('manual_send', code)
    # rec = new_values('manual_send',
    #                  ('code', 'grid', 'direction'),
    #                  (code, grid_id, direction))

    print('rec send code', rec, code)

    session['direction'] = direction
    session['send_manual_id'] = rec
    # session['m_send_code'] = code

    manual_obj = {
        'send_manual_id': session['send_manual_id'],
        # 'direction': session['direction'],
        'direction': direction,
        'send_code': code,
    }

    return jsonify(manual_obj)


@app.route('/manual-receive/<code>')
def m_receive(code):
    rec = new_code('manual_receive', code)

    obstacle = True
    if code[-1] == '0':
        obstacle = False

    session['obstacle'] = obstacle
    session['receive_manual_id'] = rec

    manual_obj = {
        'receive_manual_id': session['receive_manual_id'],
        'obstacle': session['obstacle'],
        'receive_code': code,
    }

    return jsonify(manual_obj)


@app.route('/get_manual_receive')
def get_manual_receive():
    code = ''
    direction = -1
    send_id, recv_id = -1, -1
    query = 'select rowid, code from manual_send order by rowid desc limit 1'
    # query = 'select rowid, code, grid, direction  from manual_send order by
    # rowid desc limit 1'
    records = query_db(query)
    if records:
        # send_id, value, grid, direction = records[0]
        send_id, code = records[0]
        # code = code[2:]

    query = 'select rowid, code from manual_receive order by rowid desc limit 1'
    records = query_db(query)
    if records:
        recv_id, code = records[0]
        code = code[2:]

    manual_obj = {
        'data': {
            'send_id': send_id,
            'recv_id': recv_id,
            'done_id': session.get('done_id', 0),
            'code': code,
            'x': -1,
            'y': -1,
            'grid_id': -1,
            'direction': direction,
            'obstacle': -1
        }
    }

    if code:
        x = int(code[:3])
        y = int(code[3:6])
        direction = code[-2]
        obstacle = code[-1]
        if x > 160:
            x = 159
        elif x < 0:
            x = 1

        if y > 200:
            y = 199
        elif y < 0:
            y = 1
        manual_obj = {
            'data': {
                'query': query,
                'send_id': send_id,
                'recv_id': recv_id,
                'done_id': session.get('done_id', 0),
                'code': code,
                'x': x,
                'y': y,
                'grid_id': '{}-{}'.format((int(x) // 40) + 1, (int(y) // 40) + 1),
                'direction': direction,
                'obstacle': obstacle,
            }
        }

    return jsonify(manual_obj)


def get_manual_send():
    # return "test auto"
    query = 'select rowid, code from manual_send order by rowid desc limit 1'
    # return query
    rec = query_db(query)[0]
    # send_id, value = rec

    return rec


###################
# auto
###################


@app.route('/auto-send/<code>')
def a_send(code):
    rec = new_code('auto_send', code)
    return jsonify(rec)


@app.route('/auto-receive/<code>')
def a_receive(code):
    rec = new_code('auto_receive', code)
    return jsonify(rec)


################
# receive
################

@app.route('/get_auto_receive')
def get_auto_receive():
    # return "test auto"
    table = 'auto_receive'
    query = 'select rowid, code from {} order by rowid desc limit 1'.format(
        table)
    # return query

    '''
    x（3）+ y（3）+ dirction + type + flag
    123|456|0|1
    0 1 2 3 4 5 6 7 8
     1 2 3|4 5 6|0|1
    '''
    records = query_db(query)
    rec = {'data': ''}
    if records:
        id, code = records[0]
        code = code[2:]

        x = int(code[:3])
        y = int(code[3:6])
        direction = code[-2]
        obstacle = code[-1]
        if x > 80:
            x = 79
        elif x < 0:
            x = 1

        if y > 100:
            y = 99
        elif y < 0:
            y = 1

        rec = {
            'data': {
                'id': id,
                'code': code,
                'x': x,
                'y': y,
                'send_id': 1,
                'grid_id': '{}-{}'.format(int(x) // 20+1, int(y) // 20+1),
                'direction': direction,
                'obstacle': obstacle,
                'done_id': session.get('done_id', 0),
            }
        }
    else:
        rec = None

    return jsonify(rec)


# @app.route('/get_auto_send')
# def get_auto_send():
#     # return "test auto"
#     query = 'select rowid, code from auto_send order by rowid desc limit 1'
#     # return query
#     rec = query_db(query)
#     return rec


@app.route('/manual')
def manual():
    # return "test auto"
    query = 'select * from manual'
    # return query
    rec = query_db(query)
    return jsonify(rec)

from app import db
from datetime import datetime

class Humidity(db.Model):
    __tablename__ = 'humidity'
    id = db.Column(db.Integer, primary_key=True)
    humidity = db.Column(db.Float(), nullable=False)
    updated_on = db.Column(db.DateTime(), default=datetime.now(), onupdate=datetime.now()) 

    def __init__(self, humidity):
        self.humidity = humidity
        self.time = datetime.now()

    def __repr__(self):
        return '<id {}>'.format(self.id)
    
    def save(self):
        db.session.add(self)
        db.session.commit()

class Temperature(db.Model):
    __tablename__ = 'temperature'
    id = db.Column(db.Integer, primary_key=True)
    temperature = db.Column(db.Float(), nullable=False)
    updated_on = db.Column(db.DateTime(), default=datetime.now(), onupdate=datetime.now()) 

    def __init__(self, temperature):
        self.temperature = temperature
        self.time = datetime.now()

    def __repr__(self):
        return '<id {}>'.format(self.id)
    
    def save(self):
        db.session.add(self)
        db.session.commit()

class Light(db.Model):
    __tablename__ = 'light'
    id = db.Column(db.Integer, primary_key=True)
    light = db.Column(db.Float(), nullable=False)
    updated_on = db.Column(db.DateTime(), default=datetime.now(), onupdate=datetime.now()) 

    def __init__(self, light):
        self.light = light
        self.time = datetime.now()

    def __repr__(self):
        return '<id {}>'.format(self.id)
    
    def save(self):
        db.session.add(self)
        db.session.commit()

class Moisture(db.Model):
    __tablename__ = 'moisture'
    id = db.Column(db.Integer, primary_key=True)
    moisture = db.Column(db.Float(), nullable=False)
    updated_on = db.Column(db.DateTime(), default=datetime.now(), onupdate=datetime.now()) 

    def __init__(self, moisture):
        self.moisture = moisture
        self.time = datetime.now()

    def __repr__(self):
        return '<id {}>'.format(self.id)
    
    def save(self):
        db.session.add(self)
        db.session.commit()

    
# {'payload': '0\x00', 'topic': 'rodneyeris/demo2eris/distance'}
# {'payload': '26.800\x00', 'topic': 'rodneyeris/demo2eris/temperature'}
# {'payload': '71.000\x00', 'topic': 'rodneyeris/demo2eris/humidity'}
# {'payload': '2411\x00', 'topic': 'rodneyeris/demo2eris/light'}
# {'payload': '22.400\x00', 'topic': 'rodneyeris/demo2eris/temperature'}
# {'payload': '60.000\x00', 'topic': 'rodneyeris/demo2eris/humidity'}
# {'payload': '0.000\x00', 'topic': 'rodneyeris/demo2eris/moisture'}
# {'payload': '"on"', 'topic': 'rodneyeris/demo2eris/relay'}
# {'payload': 'on\n', 'topic': 'rodneyeris/demo2eris/relay'}

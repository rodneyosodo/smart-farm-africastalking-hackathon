from app import db
from datetime import datetime

class Humidity(db.Model):
    __tablename__ = 'humidity'
    id = db.Column(db.Integer, primary_key=True)
    humidity = db.Column(db.Float(), nullable=False)
    updated_on = db.Column(db.DateTime()) 

    def __init__(self, humidity):
        self.humidity = humidity
        self.updated_on = datetime.now()

    def __repr__(self):
        return '<id {}: {}>'.format(self.id, self.updated_on)
    
    def save(self):
        db.session.add(self)
        db.session.commit()

class Temperature(db.Model):
    __tablename__ = 'temperature'
    id = db.Column(db.Integer, primary_key=True)
    temperature = db.Column(db.Float(), nullable=False)
    updated_on = db.Column(db.DateTime()) 

    def __init__(self, temperature):
        self.temperature = temperature
        self.updated_on = datetime.now()

    def __repr__(self):
        return '<id {}>'.format(self.id)
    
    def save(self):
        db.session.add(self)
        db.session.commit()

class Light(db.Model):
    __tablename__ = 'light'
    id = db.Column(db.Integer, primary_key=True)
    light = db.Column(db.Float(), nullable=False)
    updated_on = db.Column(db.DateTime()) 

    def __init__(self, light):
        self.light = light
        self.updated_on = datetime.now()

    def __repr__(self):
        return '<id {}>'.format(self.id)
    
    def save(self):
        db.session.add(self)
        db.session.commit()

class Moisture(db.Model):
    __tablename__ = 'moisture'
    id = db.Column(db.Integer, primary_key=True)
    moisture = db.Column(db.Float(), nullable=False)
    updated_on = db.Column(db.DateTime()) 

    def __init__(self, moisture):
        self.moisture = moisture
        self.updated_on = datetime.now()

    def __repr__(self):
        return '<id {}>'.format(self.id)
    
    def save(self):
        db.session.add(self)
        db.session.commit()

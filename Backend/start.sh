export DATABASE_URL="postgresql://postgres:postgres@127.0.0.1:1001/demo"
export APP_SETTINGS="config.DevelopmentConfig"
python3 manage.py db init
python3 manage.py db migrate
python3 manage.py db upgrade
python3 manage.py runserver

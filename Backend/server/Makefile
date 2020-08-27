export-config:
	export DATABASE_URL="postgresql://postgres:postgres@127.0.0.1:1001/demo"
	export APP_SETTINGS="config.DevelopmentConfig"
	
start-postgres:
	docker run --rm -d -p 127.0.0.1:1001:5432 -e "POSTGRES_USER"="postgres" -e POSTGRES_PASSWORD=postgres --name postgres postgres

start-app:
	./start.sh

start-ngrok:
	ngrok http 5000
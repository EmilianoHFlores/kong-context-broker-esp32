from OrionManager import OrionManager
import os
import datetime

if __name__ == "__main__":
    client_id = os.getenv("KEYCLOAK_CLIENT_ID")
    client_secret = os.getenv("KEYCLOAK_CLIENT_SECRET")
    keycloak_url = os.getenv("KEYCLOAK_URL")
    orion_url = os.getenv("ORION_URL")
    kong_url = os.getenv("KONG_URL")

    if not client_id or not client_secret:
        print("Client ID and Client Secret must be provided in the .env file.")
        exit(1)

    manager = OrionManager(client_id, client_secret, keycloak_url, orion_url, kong_url)
    
    entity_type = "ParticleSensor"
    
    # build object
    new_entity = manager.build_environment_entity([10, 20, 30], datetime.datetime.now())
    # publish
    print(new_entity.data)
    manager.create_entity(new_entity.data)
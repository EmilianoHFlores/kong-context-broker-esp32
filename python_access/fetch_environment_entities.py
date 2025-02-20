from OrionManager import OrionManager
import os

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
    
    entities = manager.fetch_entities(entity_type)
    
    print(entities)
    print(f"Total entities: {len(entities)}")
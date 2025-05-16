from OrionManager import OrionManager
from datetime import datetime
import os
from dateutil.relativedelta import relativedelta

# def filter_entities_current_month(entities):
#     now = datetime.now()
#     current_year = now.year
#     current_month = now.month
#     filtered = []

#     for entity in entities:
#         entity_date = None

#         # Caso 1: dateObserved
#         if 'dateObserved' in entity:
#             try:
#                 date_str = entity['dateObserved'].get('value')
#                 entity_date = datetime.fromisoformat(date_str.replace("Z", "+00:00"))
#             except Exception as e:
#                 print(f"Error parseando 'dateObserved': {e}")
        
#         # Caso 2: timestamp
#         elif 'timestamp' in entity:
#             try:
#                 ts_value = entity['timestamp'].get('value')
#                 if isinstance(ts_value, str):
#                     ts_value = int(ts_value)
#                 entity_date = datetime.fromtimestamp(ts_value)
#             except Exception as e:
#                 print(f"Error parseando 'timestamp': {e}")

#         # Filtro del mes actual
#         if entity_date and entity_date.year == current_year and entity_date.month == current_month:
#             filtered.append(entity)

#     return filtered



def filter_entities_last_hour(entities):
    now = datetime.now()
    one_hour_ago = now - relativedelta(hours=1)
    filtered = []

    for entity in entities:
        entity_date = None

        # Caso 1: dateObserved
        if 'dateObserved' in entity:
            try:
                date_str = entity['dateObserved'].get('value')
                entity_date = datetime.fromisoformat(date_str.replace("Z", "+00:00"))
            except Exception as e:
                print(f"Error parseando 'dateObserved': {e}")
        
        # Caso 2: timestamp (epoch time)
        elif 'timestamp' in entity:
            try:
                ts_value = entity['timestamp'].get('value')
                if isinstance(ts_value, str):
                    ts_value = int(ts_value)
                entity_date = datetime.fromtimestamp(ts_value)
            except Exception as e:
                print(f"Error parseando 'timestamp': {e}")

        # Filtro: última hora
        if entity_date and entity_date >= one_hour_ago:
            filtered.append(entity)

    return filtered


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

    for entity_type in ["ParticleSensor", "EnvironmentSensor"]:
        entities = manager.fetch_entities(entity_type)
        # filtered_entities = filter_entities_current_month(entities)

        # print(f"\nFiltered {entity_type} Entities (Last 3 Months):")
        # print(filtered_entities)
        # print(f"Total {entity_type} Entities (Last 3 Months): {len(filtered_entities)}")
        
        
        filtered_entities = filter_entities_last_hour(entities)

        print(f"\nFiltered {entity_type} Entities (Last Hour):")
        print(filtered_entities)
        print(f"Total {entity_type} Entities (Last Hour): {len(filtered_entities)}")


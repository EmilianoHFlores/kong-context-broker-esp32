import os
import uuid
import requests
import json
import time
from datetime import datetime, timezone, timedelta

from Entity import Entity
from save_pickle import save_pickle
from file import ensure_dir

def parse_date(entity):
    return datetime.fromisoformat(entity['observedAt']['value'])

# load env at .env
from dotenv import load_dotenv
load_dotenv()

class OrionManager:
    def __init__(self, client_id: str, client_secret: str, keycloak_url: str, orion_url: str, kong_url: str):
        """
        Initialize the OrionManager class.

        Args:
            client_id (str): Keycloak client ID.
            client_secret (str): Keycloak client secret.
            keycloak_url (str): Keycloak token URL.
            orion_url (str): Orion Context Broker base URL.
        """
        self.client_id = client_id
        self.client_secret = client_secret
        self.keycloak_url = keycloak_url.rstrip("/")
        self.orion_url = orion_url.rstrip("/")
        self.kong_url = kong_url.rstrip("/")
        self.token = None
        self.token_expiry = 0

    def obtain_token(self):
        """
        Obtain an access token from Keycloak using client credentials.
        """
        token_data = {
            "grant_type": "client_credentials",
            "client_id": self.client_id,
            "client_secret": self.client_secret,
        }

        if self.token_expiry < time.time():
            try:
                response = requests.post(self.keycloak_url, data=token_data)
                response.raise_for_status()
                token_json = response.json()
                self.token = token_json.get("access_token")
                expires_in = token_json.get("expires_in", 0)
                self.token_expiry = time.time() + expires_in - 10
                print("Access token obtained successfully.")
            except requests.exceptions.RequestException as e:
                print(f"Error obtaining token: {e}")
                raise

    def get_token(self):
        """
        Get a valid token, refreshing it if necessary.
        """
        if not self.token or time.time() > self.token_expiry:
            self.obtain_token()
        return self.token
    
    def get_modify_headers(self):
        return {
            "Authorization": f"Bearer {self.get_token()}",
            "Content-Type": "application/ld+json"
        }
    
    def fetch_and_filter_entities(self, entity_type: str, path_prefix: str = "galeria", batch_size: int = 1000) -> list[str]:
        """
        Fetch and filter entities based on a prefix for the 'path' field.

        Args:
            entity_type (str): Type of entities to fetch.
            path_prefix (str): Prefix for filtering 'path' values.
            batch_size (int): Maximum number of entities per batch.

        Returns:
            List[str]: List of filtered paths.
        """
        offset = 0
        filtered_paths = []

        headers = {
            "Content-Type": "application/json",
            "Accept": "application/json",
            "Authorization": f"Bearer {self.get_token()}"
        }

        while True:
            params = {"type": entity_type, "limit": batch_size, "offset": offset}
            try:
                response = requests.get(f"{self.orion_url}/v1/entities", params=params, headers=headers)
                response.raise_for_status()
                entities = response.json()

                if not entities:
                    break  # No more entities available

                for entity in entities:
                    path_value = entity.get("path", {}).get("value", "")
                    if path_value.startswith(path_prefix):
                        filtered_paths.append(path_value)

                offset += batch_size
                print(f"Processed {offset} entities...")

            except requests.exceptions.RequestException as e:
                print(f"Error fetching entities: {e}")
                break

        return filtered_paths
    
    def fetch_entities(self, entity_type: str, batch_size: int = 1000) -> list[dict]:
        """
        Fetch entities of a given type.

        Args:
            entity_type (str): Type of entities to fetch.
            batch_size (int): Maximum number of entities per batch.

        Returns:
            List[dict]: List of entities.
        """
        offset = 0
        entities = []

        headers = {
            "Content-Type": "application/json",
            "Accept": "application/json",
            "Authorization": f"Bearer {self.get_token()}"
        }

        while True:
            params = {"type": entity_type, "limit": batch_size, "offset": offset}
            try:
                response = requests.get(f"{self.orion_url}/v1/entities", params=params, headers=headers)
                response.raise_for_status()
                entities_batch = response.json()

                if not entities_batch:
                    break  # No more entities available

                entities.extend(entities_batch)
                offset += batch_size
                print(f"Processed {offset} entities...")

            except requests.exceptions.RequestException as e:
                print(f"Error fetching entities: {e}")
                break

        return entities

    def save_filtered_paths(self, paths: list[str], output_file: str = "filtered_paths.json") -> None:
        """
        Save filtered paths to a JSON file.

        Args:
            paths (List[str]): List of paths to save.
            output_file (str): Output file name.
        """
        try:
            with open(output_file, "w") as json_file:
                json.dump(paths, json_file, indent=4)
            print(f"Filtered paths saved to '{output_file}'.")
        except IOError as e:
            print(f"Error saving paths to file: {e}")
            
    def build_temperature_entity(self, value: float, observed_at: datetime) -> dict:
        """
        Create a temperature entity.

        Args:
            location: tuple -> Coordinates (longitude, latitude).
            value: float -> Temperature value.
            observed_at: datetime -> Observation timestamp.
        Returns:
            dict: Entity data.
        """
        print('Creating temperature entity')

        unique_id = str(uuid.uuid4())
        e = Entity("Temperature", f"urn:ngsi-ld:Temperature:{unique_id}")
        e.prop("temperature", value)
        e.prop("dateObserved", observed_at.isoformat())

        print('Temperature entity created')
        return e
    
    def build_environment_entity(self, values:list[float], observed_at: datetime) -> dict:
        """
        Create a temperature entity.

        Args:
            location: tuple -> Coordinates (longitude, latitude).
            value: float -> Temperature value.
            observed_at: datetime -> Observation timestamp.
        Returns:
            dict: Entity data.
        """
        print('Creating environment entity')

        unique_id = str(uuid.uuid4())
        e = Entity("ParticleSensor", f"urn:ngsi-ld:ParticleSensor:{unique_id}")
        e.prop("PM10", values[0])
        e.prop("PM2.5", values[1])
        e.prop("PM1", values[2])
        e.prop("dateObserved", observed_at.isoformat())

        print('ParticleSensor entity created')
        return e
    
    def create_entity(self, entity_data: dict) -> None:
        """
        Create an entity in the Orion-LD context broker.
        """
        print('Creating entity')

        response = requests.post(
            self.kong_url, headers=self.get_modify_headers(), data=json.dumps(entity_data))
        response.raise_for_status()

        print("Entity created successfully.")
    
    def query_type(self, entity_type, offset=0, entities_limit=2000) -> list:
        params = {
            "type": entity_type,
            "limit": 1000,
            "offset": offset
        }
        if offset + 1000 > entities_limit:
            params["limit"] = entities_limit - offset
            if params["limit"] <= 0:
                return []

        response = requests.get(
            self.kong_url, headers=self.get_headers(), params=params)

        if response.status_code == 200:
            elements = response.json()
            if len(elements) == 1000:
                elements += self.query_type(entity_type,
                                            offset + 1000, entities_limit)
            return elements
        else:
            print(
                f"Failed to fetch entities: {response.status_code} {response.text}")
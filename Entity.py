class Entity:

    def __init__(self,
                 entity_type: str,
                 entity_id: str) -> None:
        """ Class to create an NGSI-LD entity 
        Args:  entity_type: str, entity_id: str
        Returns: None

        """
        self.data = {
            "id": f"urn:ngsi-ld:{entity_type}:{entity_id}",
            "type": entity_type,
            "@context": [
                "https://uri.etsi.org/ngsi-ld/v1/ngsi-ld-core-context-v1.6.jsonld"
            ]
        }

    def prop(self, name, value):
        self.data[name] = {
            "type": "Property",
            "value": value
        }

    def rel(self, name, value):
        self.data[name] = {
            "type": "Relationship",
            "object": value
        }

    def insert_key(self, name, value):
        self.data[name] = value

    @staticmethod
    def add_context(entity: dict):
        entity["@context"] = [
            "https://uri.etsi.org/ngsi-ld/v1/ngsi-ld-core-context-v1.6.jsonld"
        ]
        return entity
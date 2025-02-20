import pickle


def save_pickle(object, path):
    with open(path, 'wb') as file:
        pickle.dump(object, file)


def load_pickle(path):
    with open(path, 'rb') as file:
        return pickle.load(file)

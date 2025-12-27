# дерево меркла

import hashlib
# узел дерева
class MerkleNode:
    def __init__(self, hash_value):
        self.hash = hash_value  # хэш узла
        self.left = None        # левый узел
        self.right = None       # правый узел

# вычислить хэш SHA-256
def calculate_hash(data):
    # преобразуем строку в байты и вычисляем хэш
    return hashlib.sha256(data.encode()).hexdigest()

# дерево Меркла - основная структура
class MerkleTree:
    def __init__(self, data_list):
        # создаём дерево из списка данных
        self.data = data_list # исходные данные
        self.leaves = [] # листья дерева
        self.root = None # корневой узел
        self.build_tree() # строим дерево
    
    def build_tree(self):
        # создаём листья - хэшируем каждый элемент
        for item in self.data:
            leaf_hash = calculate_hash(item)
            self.leaves.append(MerkleNode(leaf_hash))
        
        # строим дерево снизу вверх
        current_level = self.leaves.copy() # начинаем с листьев
        while len(current_level) > 1: # пока не останется корень
            next_level = []
            # обрабатываем узлы парами
            for i in range(0, len(current_level), 2):
                left = current_level[i]
                # если нет пары, используем тот же узел
                right = current_level[i+1] if i+1 < len(current_level) else left
                # объединяем хэши
                combined = left.hash + right.hash
                parent_hash = calculate_hash(combined)
                # создаём родительский узел
                parent = MerkleNode(parent_hash)
                parent.left = left
                parent.right = right
                next_level.append(parent)
            current_level = next_level # переходим на уровень выше
        self.root = current_level[0] if current_level else None
    def get_root(self):
        # возвращаем корневой хэш (представляет все данные)
        return self.root.hash if self.root else ""
    def verify(self, item):
        # проверяем есть ли элемент в дереве
        item_hash = calculate_hash(item) # хэш искомого элемента
        for leaf in self.leaves:
            if leaf.hash == item_hash:
                return True
        return False
    def get_info(self):
        # информация о дереве
        height = 0
        n = len(self.leaves)
        while n > 0:
            height += 1
            n //= 2
        return {
            'items': len(self.data), # сколько элементов
            'root': self.get_root()[:16] + "...", # начало корня
            'height': height # высота дерева
        }

# тест
if __name__ == "__main__":
    test_data = ["тест1", "тест2", "тест3"]
    tree = MerkleTree(test_data)
    print("тест:")
    print(f"корень: {tree.get_root()[:20]}...")
    print(f"проверка 'тест1': {tree.verify('тест1')}")
    print(f"проверка 'фейк': {tree.verify('фейк')}")
    info = tree.get_info()
    print(f"элементов: {info['items']}")
    print(f"высота: {info['height']}")
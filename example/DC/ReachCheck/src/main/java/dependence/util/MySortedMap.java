package dependence.util;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.TreeMap;

public class MySortedMap<K, V> {
	private final TreeMap<K, Collection<V>> container;

	public MySortedMap() {
		container = new TreeMap<>();
	}

	public void add(K k, V v) {
		Collection<V> set = container.computeIfAbsent(k, k1 -> new ArrayList<V>());
		set.add(v);
	}

	public int size() {
		return container.size();
	}

	public List<V> flat() {
		List<V> result = new ArrayList<V>();
		for (K k : container.keySet()) {
			result.addAll(container.get(k));
		}
		return result;
	}
}

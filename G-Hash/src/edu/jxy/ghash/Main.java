package edu.jxy.ghash;

import java.io.FileNotFoundException;
import java.util.ArrayList;

public class Main {

	public static void main(String[] args) {
		System.out.println(System.getProperty("user.dir"));
		String fileName = "../assistant/data/AIDO99SD-16000";
		String queryfileName = "../assistant/data/query";
		double ans = 0;
		int runtime = 1;
		int w = runtime;
		while (w > 0) {
			ans += run(fileName, queryfileName);
			w--;
		}
		System.out.println(ans / runtime);

	}

	public static double run(String fileName, String queryfileName) {
		long begintime = System.currentTimeMillis();
		Unifier unifier = Unifier.getInstance();
		// System.out.println(unifier.calcSimilarity("a,1,0,1", "a,1,0,1"));
		ReadFile.readGraph(fileName);
		unifier.buildUnifier();
		System.out.println(unifier.getAllNodeNum());
		System.out.println(unifier.getAllEdgeNum());
		Query query;
		try {
			query = new Query(queryfileName);
			ArrayList<Integer> result = query.getKResult();
			// System.out.println(result);
			unifier.clear();
			return ((System.currentTimeMillis() - begintime) / 1000.0);
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return 0;
		}
	}
}

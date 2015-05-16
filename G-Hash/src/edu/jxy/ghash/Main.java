package edu.jxy.ghash;

import java.io.FileNotFoundException;
import java.util.ArrayList;

public class Main {

	public static void main(String[] args) {
		System.out.println(System.getProperty("user.dir"));
		String fileName = "../assistant/data/AIDO99SD-16000";
		String queryfileName = "../assistant/data/query";
		Unifier unifier = Unifier.getInstance();
		ReadFile.readGraph(fileName);
		unifier.buildUnifier();
		Query query;
		try {
			query = new Query(queryfileName);
			ArrayList<Integer> result = query.getKResult();
			System.out.println(result);
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}
}

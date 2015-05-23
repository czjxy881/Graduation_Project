package edu.jxy.ghash;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Scanner;

public class Query {

	class myComparator implements Comparator<Integer> {

		@Override
		public int compare(Integer o1, Integer o2) {
			if (Math.abs(ResultList[o1] - ResultList[o2]) < 10e-5) {
				return 0;
			} else if (ResultList[o1] < ResultList[o2]) {
				return -1;
			} else {
				return 1;
			}
		}

	}

	private int K;

	private List<String> QueryBagVector;

	private Graph QueryGraph;

	/**
	 * 保存相似度
	 */
	private double[] ResultList;

	private Unifier unifier;

	public Query(String filename) throws FileNotFoundException {
		unifier = Unifier.getInstance();
		QueryBagVector = new ArrayList<String>();
		Scanner scanner = new Scanner(new File(filename));
		QueryGraph = new Graph();
		QueryGraph.read(scanner);
		BuildQueryBagVector();
		calcResult();
	}

	public void BuildQueryBagVector() {
		for (int i = 0; i < QueryGraph.getNumNode(); i++) {
			QueryBagVector.add(unifier.getBag(i, QueryGraph));
		}
	}

	public void calcResult() {
		int graphSize = unifier.getGraphCount();
		ResultList = new double[graphSize];
		int i, j;
		ArrayList<String> bags = unifier.getBags();
		int bagsize = unifier.getBagNum();
		for (String bag : QueryBagVector) {
			for (j = 0; j < bagsize; j++) {
				double similarity = unifier.calcSimilarity(bag, bags.get(j));
				for (i = 0; i < graphSize; i++) {
					if (unifier.DataBaseMap[i][j] != 0) {
						ResultList[i] += unifier.DataBaseMap[i][j] * similarity;
					}
				}
			}
		}
		for (i = 0; i < graphSize; i++) {
			ResultList[i] = unifier.getGraphNodeCount(i) - 2 * ResultList[i];
		}
	}

	public ArrayList<Integer> getKResult() {
		int graphSize = unifier.getGraphCount();
		int i;
		ArrayList<Integer> result = new ArrayList<Integer>();
		for (i = 0; i < graphSize; i++) {
			result.add(i);
		}
		myComparator comparator = new myComparator();
		Collections.sort(result, comparator);
		return result;
	}

	public Graph getQueryGraph() {
		return QueryGraph;
	}

	public void setQueryGraph(Graph queryGraph) {
		QueryGraph = queryGraph;
	}
}

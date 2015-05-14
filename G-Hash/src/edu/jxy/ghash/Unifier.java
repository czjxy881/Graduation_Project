package edu.jxy.ghash;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.TreeMap;

public class Unifier {

	private HashMap<String, Integer> NodeLabelMap;

	private TreeMap<String, Integer> BagMap;

	private int[][] DataBaseMap;

	private double[][] Similarity;

	private ArrayList<Graph> GraphList;
	private boolean isUpdated;
	private ArrayList<Integer> EveryLabelNum;

	private int LabelNum;
	private int BagNum;

	private Graph[] graph;

	public Unifier() {
		NodeLabelMap = new HashMap<String, Integer>();
		BagMap = new TreeMap<String, Integer>();
		isUpdated = false;
		LabelNum = 0;
		BagNum = 0;
		EveryLabelNum = new ArrayList<Integer>();
	}

	public void addGraph(Graph graph) {
		GraphList.add(graph);
		isUpdated = false;
	}

	public void buildUnifier() {

	}

	private void readAllNode() {
		NodeLabelMap.clear();
		LabelNum = 0;
		for (Graph graph : GraphList) {
			String[] labelList = graph.getNodeLabel();
			for (String label : labelList) {
				if (!NodeLabelMap.containsKey(label)) {
					LabelNum++;
					NodeLabelMap.put(label, LabelNum);
					EveryLabelNum.add(1);
				} else {
					int index = NodeLabelMap.get(label);
					EveryLabelNum.add(index, EveryLabelNum.get(index) + 1);
				}
			}
		}
	}

	private void readGraph(int index) {
		Graph graph = GraphList.get(index);
		int num = graph.getNumNode();
		int i;
		for (i = 0; i < LabelNum; i++)
			for (int i = 0; i < num; i++) {
				String bag = getBag(i, graph);
				if (BagMap.containsKey(bag) == false) {
					BagMap.put(bag, BagNum);
					BagNum++;
				}

			}

	}

	public String getBag(int nodeIndex, Graph graph) {
		boolean[][] map = graph.getMap();
		int i;
		int num[] = new int[LabelNum];
		for (i = 0; i < num.length; i++) {
			num[i] = 0;
		}
		String bag = graph.getNodeLabel(nodeIndex);
		for (i = 0; i < map[nodeIndex].length; i++) {
			if (map[nodeIndex][i] == true) {
				num[NodeLabelMap.get(graph.getNodeLabel(i))]++;
			}
		}
		for (i = 0; i < num.length; i++) {
			bag += "," + String.valueOf(num[i]);
		}
		return bag;
	}

	public void calcSimiliarity() {

	}

	public void getSimiliarity() {

	}

	private double calcSimilarity(String Bag1, String Bag2) {
		return 0;
	}

	public void addBag(String bag) {

	}

	private void serialize() {

	}

}

package edu.jxy.ghash;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.TreeMap;

public class Unifier {

	private static Unifier unifier = null;

	public static Unifier getInstance() {
		if (unifier == null) {
			unifier = new Unifier();
		}
		return unifier;
	}
	private TreeMap<String, Integer> BagMap;

	private int BagNum;

	private ArrayList<String> Bags;

	public int[][] DataBaseMap;

	/**
	 * 用来记录每个标签个数，方便动态增删
	 */
	private ArrayList<Integer> EveryLabelNum;
	private ArrayList<Graph> GraphList;
	private boolean isUpdated;

	private int LabelNum;
	private HashMap<String, Integer> NodeLabelMap;

	private double[][] Similarity;

	private Unifier() {
		NodeLabelMap = new HashMap<String, Integer>();
		BagMap = new TreeMap<String, Integer>();
		isUpdated = false;
		LabelNum = 0;
		BagNum = 0;
		Bags = new ArrayList<String>();
		EveryLabelNum = new ArrayList<Integer>();
		GraphList = new ArrayList<Graph>();
	}

	public void addBag(String bag) {

	}

	public void addGraph(Graph graph) {
		GraphList.add(graph);
		isUpdated = false;
	}

	/**
	 * 在addgraph完成后建立数据库。包括建立NodelLabelMap,BagMap及DataBaseMap
	 */
	private void buildDataBase() {
		readAllNode();
		readGraphBags();
		countGraphBags();
	}

	private void buildSimiliarity() {
		// TODO Auto-generated method stub

	}

	public void buildUnifier() {
		buildDataBase();
		buildSimiliarity();
	}

	public double calcSimilarity(String Bag1, String Bag2) {
		return 1;
		// FIX:
	}

	public void clear() {
		unifier = null;
	}

	private void countGraphBags() {
		DataBaseMap = new int[GraphList.size()][BagNum];// todo：检测是否为0
		for (int index = 0; index < GraphList.size(); index++) {
			Graph graph = GraphList.get(index);
			int num = graph.getNumNode();
			int i;
			for (i = 0; i < num; i++) {
				int bagIndex = graph.getBagIndxByIndex(i);
				DataBaseMap[index][bagIndex]++;
			}

		}
	}

	public String getBag(int nodeIndex, Graph graph) {
		boolean[][] map = graph.getMap();
		int i;
		int num[] = new int[LabelNum];// TODO: 测试是否为0
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

	public int getBagNum() {
		return BagNum;
	}

	public ArrayList<String> getBags() {
		return Bags;
	}

	public int getGraphCount() {
		return GraphList.size();
	}

	public int getGraphNodeCount(int index) {
		return GraphList.get(index).getNumNode();
	}

	public void getSimiliarity() {

	}

	private void readAllNode() {
		NodeLabelMap.clear();
		LabelNum = 0;
		for (Graph graph : GraphList) {
			String[] labelList = graph.getNodeLabel();
			for (String label : labelList) {
				if (!NodeLabelMap.containsKey(label)) {
					NodeLabelMap.put(label, LabelNum);
					LabelNum++;
					EveryLabelNum.add(1);
				} else {
					int index = NodeLabelMap.get(label);
					EveryLabelNum.set(index, EveryLabelNum.get(index) + 1);
				}
			}
		}
	}

	// todo:
	// 分离成两个来适应graph的扩展性，首先readGraphBags得到所有bag，然后countGraphBags来统计各图bag得到DataBase。但是这样明显增加了运算量
	private void readGraphBags() {
		for (int index = 0; index < GraphList.size(); index++) {
			Graph graph = GraphList.get(index);
			int num = graph.getNumNode();
			int i;
			for (i = 0; i < num; i++) {
				String bag = getBag(i, graph);
				if (BagMap.containsKey(bag) == false) {
					BagMap.put(bag, BagNum);
					BagNum++;
					Bags.add(bag);
				}
				graph.setBagIndxByIndex(i, BagMap.get(bag));
			}
		}
	}

	private void serialize() {

	}

}

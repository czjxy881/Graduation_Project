package edu.jxy.ghash;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;

public class ReadFile {

	private static Scanner scanner;

	public static void readGraph(String fileName) {
		try {
			scanner = new Scanner(new File(fileName));
			while (scanner.hasNext()) {
				Graph graph = new Graph();
				graph.read(scanner);
				Unifier.getInstance().addGraph(graph);
			}
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

}

package edu.asu.plp.tool.prototype.view.menu.options.details;

import edu.asu.plp.tool.prototype.model.ApplicationSetting;
import edu.asu.plp.tool.prototype.model.SettingUtil;

/**
 * @author Nesbitt, Morgan on 2/27/2016.
 */
public class SimulatorSettingDetails
{
	public static final SimulatorSettingDetails DEFAULT = SimulatorSettingDetails.defaultDetails();

	private String simulationSpeedMillisecondsCycle;
	private String allowExecutionOfNonInstructionMemory;
	private String assumeZeroOnReadsFromUninitializedMemory;

	private SimulatorSettingDetails()
	{

	}

	public SimulatorSettingDetails( String simulationSpeedMillisecondsCycle,
			String allowExecutionOfNonInstructionMemory, String assumeZeroOnReadsFromUninitializedMemory )
	{
		this.simulationSpeedMillisecondsCycle = simulationSpeedMillisecondsCycle;
		this.allowExecutionOfNonInstructionMemory = allowExecutionOfNonInstructionMemory;
		this.assumeZeroOnReadsFromUninitializedMemory = assumeZeroOnReadsFromUninitializedMemory;
	}

	public SimulatorSettingDetails( SimulatorSettingDetails details )
	{
		this(details.simulationSpeedMillisecondsCycle, details.allowExecutionOfNonInstructionMemory,
			 details.assumeZeroOnReadsFromUninitializedMemory);
	}

	private static SimulatorSettingDetails defaultDetails()
	{
		SimulatorSettingDetails details = new SimulatorSettingDetails();

		ApplicationSetting setting = ApplicationSetting.SIMULATOR_SPEED;
		details.simulationSpeedMillisecondsCycle =
				SettingUtil.loadRequiredSavedSettingDefaultIfNotPresent(setting);

		setting = ApplicationSetting.SIMULATOR_ALLOW_EXECUTION_OF_NON_INSTRUCTION_MEMORY;
		details.allowExecutionOfNonInstructionMemory =
				SettingUtil.loadRequiredSavedSettingDefaultIfNotPresent(setting);

		setting = ApplicationSetting.SIMULATOR_ASSUME_ZERO_ON_READS_FROM_UNINITIALIZED_MEMORY;
		details.assumeZeroOnReadsFromUninitializedMemory =
				SettingUtil.loadRequiredSavedSettingDefaultIfNotPresent(setting);

		return details;
	}

	public String getSimulationSpeedMillisecondsCycle()
	{
		return simulationSpeedMillisecondsCycle;
	}

	public String getAllowExecutionOfNonInstructionMemory()
	{
		return allowExecutionOfNonInstructionMemory;
	}

	public String getAssumeZeroOnReadsFromUninitializedMemory()
	{
		return assumeZeroOnReadsFromUninitializedMemory;
	}
}
